#include <cqde/types/ui/ViewportManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/systems/CullingSystem.hpp>

#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/math_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <ImGuizmo.h>
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
  if ( ImGuizmo::IsUsing() )
    return false;

  const auto index  = viewportIndex(cameraId);

  if ( index < 0 )
    return false;

  const auto windowTitle = "Viewport " + std::to_string(index) + "##viewport";

  const auto window = ImGui::FindWindowByName(windowTitle.c_str());

  if ( window == nullptr )
    return false;

  const auto context = ImGui::GetCurrentContext();
  const auto contextWindow = ImGui::GetCurrentWindowRead();

  context->CurrentWindow = window;

  const auto rectMin = window->InnerClipRect.Min;
  const auto rectMax = window->InnerClipRect.Max;

  bool result = ImGui::IsWindowHovered() &&
                ImGui::IsMouseHoveringRect(rectMin, rectMax);

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
  {
    ImGui::End(); // Viewports
    ui_show_viewport_windows(registry);
    return;
  }

  if ( ImGui::Button("+##viewportAdd") )
    mViewports.push_back({});

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "ViewportsList", 1, tableFlags) )
  {
    ImGui::TableNextColumn();

    for ( auto iter = mViewports.begin();
          iter != mViewports.end();
          ++iter )
    {
      const auto index = std::distance(mViewports.begin(), iter);

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      if ( ImGui::Selectable(("Viewport " + std::to_string(index)).c_str(),
                             false, flags) )
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
         registry.all_of <Camera, Transform, SceneNode> (eCamera) == true )
    {
      const float windowRatio = windowSize.x / windowSize.y;

      const auto& [cCamera, cCameraNode, cCameraTransform] = registry.get <Camera, SceneNode, Transform> (eCamera);

      cCamera.zBuffer.clear();

      glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
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

      ImGui::Checkbox("Orbit cam", &mGizmoCubeOrbit);

      if ( mGizmoCubeOrbit == true )
      {
        ImGui::SetNextItemWidth(comboWidth);
        ImGui::DragFloat("Orbit distance", &mGizmoCubeOrbitDistance, 0.1f,
                         glm::epsilon <float> (),
                         std::numeric_limits <float>::max());
      }

      ImGuizmo::SetID(viewportIndex);

      ImGuizmo::SetRect(camViewport.x, camViewport.y, camViewport.z, camViewport.w);
      ImGuizmo::SetDrawlist();

      ImGuizmo::SetOrthographic(cCamera.projectionType == Camera::Projection::Orthographic);

      const auto gizmoSize = comboWidth * 0.5f;

      const auto cursorPos = ImGui::GetMousePos();
      const auto rectPos = ImGui::GetCursorScreenPos();

      const bool gizmoCubeHovered = pointInRect({cursorPos.x, cursorPos.y},
                                                {rectPos.x - 1, rectPos.x + gizmoSize + 1,
                                                rectPos.y - 1, rectPos.y + gizmoSize + 1});

      if ( ImGui::BeginChild("##gizmoCube", {gizmoSize, gizmoSize}, true) )
      {
        if ( ImGuizmo::IsUsing() == false ||
             gizmoCubeHovered == false )
        {
          const auto camViewPrev = camView;

          ImGuizmo::ViewManipulate(glm::value_ptr(camView), mGizmoCubeOrbitDistance,
                                   ImGui::GetWindowPos(),
                                   ImGui::GetWindowSize(),
                                   ImGui::GetColorU32(ImGuiCol_WindowBg));

          if ( camView != camViewPrev &&
               (mGizmoCubeUsingIndex == viewportIndex ||
                mGizmoCubeUsingIndex == -1) )
          {
            camView = glm::inverse(camView);

            glm::vec3 camTranslation = camView[3];
            auto camOrientation = glm::toQuat(camView);

            if ( glm::all(glm::isfinite(camView[3])) == true &&
                 glm::all(glm::isfinite(glm::eulerAngles(camOrientation))) == true )
            {
              camOrientation = ToLocalSpace(camOrientation, registry,
                                            cCameraTransform, cCameraNode);

              if ( mGizmoCubeOrbit == true )
              {
                camTranslation = ToLocalSpace(camTranslation, registry,
                                              cCameraTransform, cCameraNode);
                cCameraTransform.translation = camTranslation;
              }

              cCameraTransform.orientation = camOrientation;
            }
            mGizmoCubeUsingIndex = viewportIndex;
          }
          else if ( mGizmoCubeUsingIndex == viewportIndex )
            mGizmoCubeUsingIndex = -1;
        }
      }
      ImGui::EndChild(); // ##gimoCube

      static ImGuizmo::OPERATION gizmoOperation {ImGuizmo::TRANSLATE};
      static ImGuizmo::MODE gizmoMode {ImGuizmo::LOCAL};

      static bool gizmoUseSnap {};
      static glm::vec3 gizmoSnap {1.0f};

      if ( ImGui::RadioButton("Local", gizmoMode == ImGuizmo::LOCAL) )
        gizmoMode = ImGuizmo::LOCAL;

      if ( gizmoOperation != ImGuizmo::SCALE )
      {
        ImGui::SameLine();
        if ( ImGui::RadioButton("World", gizmoMode == ImGuizmo::WORLD) )
          gizmoMode = ImGuizmo::WORLD;
      }

      if ( ImGui::RadioButton("Translate",
                              gizmoOperation == ImGuizmo::TRANSLATE) )
        gizmoOperation = ImGuizmo::TRANSLATE;

      if ( ImGui::RadioButton("Rotate",
                              gizmoOperation == ImGuizmo::ROTATE) )
        gizmoOperation = ImGuizmo::ROTATE;

      if ( ImGui::RadioButton("Scale",
                              gizmoOperation == ImGuizmo::SCALE) )
      {
        gizmoMode = ImGuizmo::LOCAL;
        gizmoOperation = ImGuizmo::SCALE;
      }

      ImGui::PushItemWidth(comboWidth);

      switch (gizmoOperation)
      {
        case ImGuizmo::TRANSLATE:
          ImGui::Checkbox("XYZ snap##gizmoSnap", &gizmoUseSnap);

          ImGui::DragFloat("##snapX", &gizmoSnap[0], 1.0f, 0.0f,
                            std::numeric_limits <float>::max());
          ImGui::DragFloat("##snapY", &gizmoSnap[1], 1.0f, 0.0f,
                            std::numeric_limits <float>::max());
          ImGui::DragFloat("##snapZ", &gizmoSnap[2], 1.0f, 0.0f,
                            std::numeric_limits <float>::max());
          break;

        case ImGuizmo::ROTATE:
          ImGui::Checkbox("Angle snap##gizmoSnap", &gizmoUseSnap);

          ImGui::DragFloat("##angleSnap", glm::value_ptr(gizmoSnap),
                           1.0f, 0.0f,
                           std::numeric_limits <float>::max());
          break;

        case ImGuizmo::SCALE:
          ImGui::Checkbox("Scale snap##gizmoSnap", &gizmoUseSnap);

          ImGui::DragFloat("##scaleSnap", glm::value_ptr(gizmoSnap),
                           1.0f, 0.0f,
                           std::numeric_limits <float>::max());
          break;

        default:
          break;
      }

      ImGui::PopItemWidth(); // comboWidth

      auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

      const auto selectedEntity = entityManagerUi.selectedEntity();

      if ( selectedEntity != entt::null  &&
           selectedEntity != eCamera &&
           (gizmoCubeHovered == false ||
            mGizmoCubeUsingIndex == -1 ||
            ImGuizmo::IsUsing() == true) )
      {
        auto [cTransform, cNode] = registry.try_get <Transform, SceneNode> (selectedEntity);

        if ( cTransform != nullptr )
        {
          auto matrixWorld = cNode == nullptr
                         ? cTransform->modelLocal()
                         : GetWorldMatrix(registry, *cTransform, *cNode);

          glm::mat4 matrixDelta {};

          if ( ImGuizmo::Manipulate( glm::value_ptr(camView),
                                     glm::value_ptr(camProjection),
                                     gizmoOperation,
                                     gizmoMode,
                                     glm::value_ptr(matrixWorld),
                                     glm::value_ptr(matrixDelta),
                                     gizmoUseSnap ? glm::value_ptr(gizmoSnap) : nullptr) )
          {
//            Guizmo bug workaround
            const auto entityPosRelative = glm::translate(camView, glm::vec3{matrixWorld[3]})[3];

            if ( entityPosRelative.z < -cCamera.zRange.first &&
                 entityPosRelative.z > -cCamera.zRange.second )
            {
              matrixWorld = ToLocalSpace(matrixWorld, registry,
                                         *cTransform, *cNode);

              glm::vec3 nodeTranslation {};
              glm::quat nodeOrientation {};
              glm::vec3 nodeScale {};
              glm::vec3 skew {};
              glm::vec4 perspective {};

              glm::decompose( matrixWorld,
                              nodeScale,
                              nodeOrientation,
                              nodeTranslation,
                              skew, perspective );


              if ( glm::all(glm::isfinite(nodeTranslation)) == true &&
                   glm::all(glm::isfinite(glm::eulerAngles(nodeOrientation))) == true &&
                   glm::all(glm::isfinite(nodeScale)) == true )
              {
                cTransform->translation = nodeTranslation;
                cTransform->orientation = nodeOrientation;
                cTransform->scale = nodeScale;
              }
            }
          }
        }
      }

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
