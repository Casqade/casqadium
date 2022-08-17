#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/types/SystemManager.hpp>
#include <cqde/systems/CullingSystem.hpp>

#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/util/logger.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::ui
{

void
ViewportManagerUi::ui_show(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::Camera;

  if ( ImGui::Begin("Viewports") == false )
  {
    ImGui::End(); // Viewports
    return;
  }

  mCameraFilter.search();

  for ( const auto&& [eCamera, cCamera, cTag] : registry.view <Camera, Tag> ().each() )
  {
    const auto camRef = EntityReference{cTag};

    if ( mCameraFilter.query(camRef.id.str()) == false )
      continue;

    bool selected {};

    if ( const auto viewport =  mViewports.find(camRef); viewport != mViewports.end() )
      selected = viewport->second;

    if ( ImGui::Selectable(cTag.id.str().c_str(), selected ) )
      mViewports[EntityReference{cTag}] = true;
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
  using types::SystemManager;

  auto& systemsManager = registry.ctx().at <SystemManager> ();

  systemsManager.activate("RenderSystem"_id);
  systemsManager.deactivate("CullingSystem"_id);

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( auto& [cameraRef, windowOpened] : mViewports )
  {
    if ( windowOpened == false )
      continue;

    if ( ImGui::Begin(cameraRef.id.str().c_str(),
                      &windowOpened, ImGuiWindowFlags_NoBackground) == false )
    {
      ImGui::End();
      continue;
    }

    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();

    const float windowRatio = windowSize.x / windowSize.y;

    const auto eCamera = cameraRef.get(registry);

    if ( eCamera == entt::null )
    {
      ImGui::End();
      continue;
    }

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

    ImGui::Text("%s", format("Z-buffer depth: {}", cCamera.zBuffer.size()).c_str());

    const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);
    const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);

    olc::renderer->ptrPGE->DrawRectDecal({windowPos.x, windowPos.y},
                                         {windowSize.x, windowSize.y},
                                         colorWindow);

    olc::renderer->ptrPGE->DrawRectDecal({camViewport.x, camViewport.y},
                                         {camViewport.z, camViewport.w},
                                         colorViewport);

    ImGui::End();
  }
}

} // namespace cqde::ui
