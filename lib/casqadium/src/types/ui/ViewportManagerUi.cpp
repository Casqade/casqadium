#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/systems/CullingSystem.hpp>

#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/util/logger.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::ui
{

bool
ViewportManagerUi::mouseOverViewport(
  const EntityId& cameraId )
{
  if ( mViewports.count(cameraId) == 0 )
    return false;

  const bool windowOpened = mViewports[cameraId];

  if ( windowOpened == false )
    return false;

  if ( ImGui::Begin((cameraId.str() + "##viewport").c_str()) == false )
  {
    ImGui::End(); // cameraId
    return false;
  }

  bool result {};

  const auto windowPos = ImGui::GetWindowPos();
  const auto windowSize = ImGui::GetWindowSize();

  if ( ImGui::IsMouseHoveringRect(windowPos, {windowPos.x + windowSize.x, windowPos.y + windowSize.y}) )
    result = true;

  ImGui::End(); // cameraId

  return result;
}

void
ViewportManagerUi::ui_show(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::Camera;

  if ( ImGui::Begin("Viewports") == false )
    return ImGui::End(); // Viewports

  mCameraFilter.search();

  ImGui::Separator();

  if ( ImGui::BeginTable( "ViewportsList", 1, ImGuiTableFlags_ScrollY,
                          {0.0f, ImGui::GetContentRegionAvail().y}) )
  {
    ImGui::TableNextColumn();

    for ( const auto&& [eCamera, cCamera, cTag] : registry.view <Camera, Tag> ().each() )
    {
      if ( mCameraFilter.query(cTag.id.str()) == false )
        continue;

      bool selected {};

      const EntityReference camRef {cTag};

      if ( const auto viewport =  mViewports.find(camRef); viewport != mViewports.end() )
        selected = viewport->second;

      if ( ImGui::Selectable(camRef.id.str().c_str(), selected ) )
      {
        mViewports[camRef] = true;
        ImGui::SetWindowFocus((camRef.id.str() + "##viewport").c_str());
      }
    }

    ImGui::EndTable(); // ViewportsList
  }

  ImGui::End(); // Viewports

  ui_show_viewport_windows(registry);
}

void
ViewportManagerUi::ui_show_viewport_windows(
  entt::registry& registry )
{
  using fmt::format;
  using compos::Camera;
  using compos::SceneNode;
  using compos::Transform;
  using compos::GeometryBuffer;
  using types::GeometryAssetManager;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( auto& [cameraRef, windowOpened] : mViewports )
  {
    if ( windowOpened == false )
      continue;

    const auto eCamera = cameraRef.get_if_valid(registry);

    if ( eCamera == entt::null ||
         registry.all_of <Camera,
                          SceneNode,
                          Transform> (eCamera) == false )
      continue;

    auto* pge = olc::renderer->ptrPGE;
    const auto defaultWindowSize = olc::vf2d{0.25f, 0.25f} * pge->GetWindowSize();

    ImGui::SetNextWindowPos({defaultWindowSize.x,
                            defaultWindowSize.y},
                            ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowSize({defaultWindowSize.x,
                              defaultWindowSize.y},
                             ImGuiCond_FirstUseEver);

    if ( ImGui::Begin((cameraRef.id.str() + "##viewport").c_str(),
                      &windowOpened,
                      ImGuiWindowFlags_NoBackground) == false ||
         windowOpened == false )
    {
      auto cCamera = registry.try_get <Camera> (eCamera);
      if ( cCamera != nullptr )
        cCamera->zBuffer.clear();

      ImGui::End(); // cameraRef.id
      continue;
    }

    const auto windowRegion = ImGui::GetContentRegionAvail();

    if ( windowRegion.x <= 0.0f || windowRegion.y <= 0.0f )
    {
      ImGui::End(); // cameraRef.id
      continue;
    }

    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();

    const float windowRatio = windowSize.x / windowSize.y;

    const auto& [cCamera, cCameraNode, cCameraTransform] = registry.get <Camera, SceneNode, Transform> (eCamera);

    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    glm::vec4 camViewport = cCamera.viewportScaled();

    const float viewportRatio = camViewport.z / camViewport.w;

    camViewport.x = windowPos.x + camViewport.x * windowSize.x;
    camViewport.y = windowPos.y + camViewport.y * windowSize.y;
    camViewport.z = windowSize.x;
    camViewport.w = windowSize.y;

    camViewport.z = std::min(camViewport.z, camViewport.z / windowRatio * viewportRatio);
    camViewport.w = std::min(camViewport.w, camViewport.w * windowRatio / viewportRatio);

    for ( const auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
            : registry.view <GeometryBuffer, SceneNode, Transform>().each() )
    {
      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView * GetWorldMatrix(registry, cTransform, cNode);

      auto vBuffer = systems::vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      cCamera.zBuffer.emplace( vBuffer, eDrawable );
    }

    ImGui::InvisibleButton("##mouseBlocker", windowRegion);

    ImGui::SetCursorPos(ImGui::GetCursorStartPos());
    ImGui::Text("%s", format("Z-buffer depth: {}", cCamera.zBuffer.size()).c_str());

    const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);
    const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);

    pge->DrawRectDecal({windowPos.x, windowPos.y},
                       {windowSize.x, windowSize.y},
                       colorWindow);

    pge->DrawRectDecal({camViewport.x, camViewport.y},
                       {camViewport.z, camViewport.w},
                       colorViewport);

    ImGui::End(); // cameraRef.id
  }
}

} // namespace cqde::ui
