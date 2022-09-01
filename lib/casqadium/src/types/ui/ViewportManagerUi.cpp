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
#include <imgui_internal.h>


namespace cqde::ui
{

int32_t
ViewportManagerUi::viewportIndex(
  const EntityId& cameraId ) const
{
  for ( auto iter = mViewports.begin();
        iter != mViewports.end();
        ++iter )
    if ( iter->id == cameraId )
      return std::distance( mViewports.begin(), iter );

  return -1;
}

bool
ViewportManagerUi::hasViewport(
  const EntityId& cameraId ) const
{
  for ( auto iter = mViewports.begin();
        iter != mViewports.end();
        ++iter )
    if ( iter->id == cameraId )
      return true;

  return false;
}

bool
ViewportManagerUi::mouseOverViewport(
  const EntityId& cameraId ) const
{
  const auto index  = viewportIndex(cameraId);

  if ( index < 0 )
    return false;

  const auto windowTitle = "Viewport " + std::to_string(index) + "##viewport";

  const auto window = ImGui::FindWindowByName(windowTitle.c_str());

  if ( window == nullptr )
    return false;

  const auto rectMin = window->Pos;

  const ImVec2 rectMax
  {
    rectMin.x + window->Size.x,
    rectMin.y + window->Size.y,
  };

  const auto context = ImGui::GetCurrentContext();
  const auto contextWindow = ImGui::GetCurrentWindowRead();

  context->CurrentWindow = window;

  bool result = ImGui::IsWindowHovered();

  context->CurrentWindow = contextWindow;
  ImGui::SetCurrentContext(context);

  return result;
}

void
ViewportManagerUi::ui_show(
  entt::registry& registry )
{
  using fmt::format;
  using compos::Tag;
  using compos::Camera;

  if ( ImGui::Begin("Viewports") == false )
    return ImGui::End(); // Viewports

  if ( ImGui::Button("+##viewportAdd") )
    mViewports.push_back({});

  ImGui::Separator();

  if ( ImGui::BeginTable( "ViewportsList", 1, ImGuiTableFlags_ScrollY,
                          {0.0f, ImGui::GetContentRegionAvail().y}) )
  {
    ImGui::TableNextColumn();

    for ( auto iter = mViewports.begin();
          iter != mViewports.end();
          ++iter )
    {
      const auto index = std::distance(mViewports.begin(), iter);

      if ( ImGui::Selectable(("Viewport " + std::to_string(index)).c_str()) )
        ImGui::SetWindowFocus(("Viewport " + std::to_string(index) + "##viewport").c_str());

      if ( ImGui::IsItemHovered() )
        ImGui::SetTooltip("%s", iter->id.str().c_str());

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
  using compos::Tag;
  using compos::Camera;
  using compos::SceneNode;
  using compos::Transform;
  using compos::GeometryBuffer;
  using types::GeometryAssetManager;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  int32_t windowToClose {-1};

  for ( auto iter = mViewports.begin();
        iter < mViewports.end();
        ++iter )
  {
    auto& cameraRef = *iter;
    const auto viewportIndex = std::distance(mViewports.begin(), iter);

    auto* pge = olc::renderer->ptrPGE;
    const auto defaultWindowSize = olc::vf2d{0.25f, 0.25f} * pge->GetWindowSize();

    ImGui::SetNextWindowPos({defaultWindowSize.x,
                            defaultWindowSize.y},
                            ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowSize({defaultWindowSize.x,
                              defaultWindowSize.y},
                             ImGuiCond_FirstUseEver);

    const std::string windowTitle = "Viewport " +
                                    std::to_string(viewportIndex) +
                                    "##viewport";

    bool windowOpened {true};

    auto eCamera = cameraRef.get_if_valid(registry);

    if ( eCamera == entt::null )
      cameraRef = null_id;

    if (  ImGui::Begin( windowTitle.c_str(), &windowOpened,
                        ImGuiWindowFlags_NoBackground) == false ||
          windowOpened == false )
    {
      windowToClose = viewportIndex;

      if ( eCamera != entt::null )
        registry.get <Camera> (eCamera).zBuffer.clear();

      ImGui::End(); // windowTitle
      continue;
    }

    const auto windowRegion = ImGui::GetContentRegionAvail();

    if ( windowRegion.x <= 0.0f ||
         windowRegion.y <= 0.0f )
    {
      ImGui::End(); // cameraRef.id
      continue;
    }

    float comboWidth {};
    comboWidth = std::min(ImGui::GetContentRegionAvail().x,
                          ImGui::CalcTextSize("camera_id_long_enough").x + ImGui::GetFrameHeightWithSpacing());

    ImGui::SetNextItemWidth(comboWidth);
    if ( ImGui::BeginCombo("##cameraIdCombo",
                           cameraRef.id.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      mCameraFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto&& [entity, cCamera, cTag] : registry.view <Camera, Tag> ().each() )
      {
        if ( mCameraFilter.query(cTag.id.str()) == false )
          continue;

        const bool selected = cameraRef.id == cTag.id;

        if ( selected == true ||
             hasViewport(cTag.id) == true )
          continue;

        if ( ImGui::Selectable(cTag.id.str().c_str(), selected) )
        {
          if ( cameraRef.id != null_id )
            registry.get <Camera> (eCamera).zBuffer.clear();

          *iter = cTag;
          eCamera = entity;
        }
      }

      ImGui::EndCombo(); // cameraIdCombo
    }

    const auto windowPos = ImGui::GetWindowPos();
    const auto windowSize = ImGui::GetWindowSize();

    if ( eCamera != entt::null &&
         registry.all_of <Camera, Transform, SceneNode> (eCamera ) == true )
    {
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

      ImGui::Text("%s", format("Z-buffer depth: {}", cCamera.zBuffer.size()).c_str());

      const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);

      pge->DrawRectDecal({camViewport.x, camViewport.y},
                         {camViewport.z, camViewport.w},
                         colorViewport);
    }

    const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);

    pge->DrawRectDecal({windowPos.x, windowPos.y},
                       {windowSize.x, windowSize.y},
                       colorWindow);

    ImGui::End(); // windowTitle
  }

  if ( windowToClose != -1 )
    mViewports.erase(mViewports.begin() + windowToClose);
}

} // namespace cqde::ui
