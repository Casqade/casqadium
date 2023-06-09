#include <cqde/types/ui/ViewportManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>

#include <cqde/ecs_helpers.hpp>
#include <cqde/math_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/systems.hpp>

#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>

#include <cqde/types/graphics/FrameReadback.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>

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
    if ( iter->camera.id == cameraId )
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
    if ( iter->camera.id == cameraId )
      return true;

  return false;
}

void
ViewportManagerUi::setGizmoOperation(
  ImGuizmo::OPERATION operation )
{
  mGizmoOperation = operation;
}

void
ViewportManagerUi::setGizmoSpace(
  ImGuizmo::MODE gizmoSpace )
{
  mGizmoSpace = gizmoSpace;
}

std::vector <Viewport>&
ViewportManagerUi::viewports()
{
  return mViewports;
}

const std::vector <Viewport>&
ViewportManagerUi::viewports() const
{
  return mViewports;
}

ImGuizmo::OPERATION
ViewportManagerUi::gizmoOperation() const
{
  return mGizmoOperation;
}

ImGuizmo::MODE
ViewportManagerUi::gizmoCoordinateSpace() const
{
  return mGizmoSpace;
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
        ImGui::SetTooltip("%s", iter->camera.id.str().c_str());
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
  using types::CasqadiumEngine;
  using types::FrameReadbackQueue;
  using compos::Tag;
  using compos::Camera;
  using compos::SceneNode;
  using compos::Transform;
  using compos::CasqadiumEditorInternal;

  std::vector <int32_t> windowsToClose {};

  auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  const auto windowSize = engine.windowSize();

  for ( auto iter = mViewports.begin();
        iter < mViewports.end();
        ++iter )
  {
    auto& cameraRef = iter->camera;
    const auto viewportIndex = std::distance(mViewports.begin(), iter);

    const auto defaultWindowSize =
      glm::vec2{0.25f, 0.25f} * glm::vec2{windowSize};

    ImGui::SetNextWindowPos(
      {defaultWindowSize.x, defaultWindowSize.y},
      ImGuiCond_FirstUseEver );

    ImGui::SetNextWindowSize(
      {defaultWindowSize.x, defaultWindowSize.y},
      ImGuiCond_FirstUseEver );

    const std::string windowTitle =
      "Viewport " + std::to_string(viewportIndex) +
      "##viewport";

    bool windowOpened {true};

    int32_t windowFlags
    {
      ImGuiWindowFlags_NoBackground,
    };

    const auto imguiWindow = ImGui::FindWindowByName(windowTitle.c_str());

    if ( imguiWindow != nullptr &&
         imguiWindow->Viewport != nullptr )
    {
      const auto windowHandle = imguiWindow->Viewport->PlatformHandle;
      const auto window = static_cast <GLFWwindow*> (windowHandle);

      if ( window != engine.window() )
      {
        windowFlags |= ImGuiWindowFlags_NoTitleBar;
        windowFlags |= ImGuiWindowFlags_NoResize;
        windowFlags |= ImGuiWindowFlags_NoMove;
//        windowFlags |= ImGuiWindowFlags_NoDocking;

        if ( window != nullptr )
          glfwSetWindowAttrib(window, GLFW_DECORATED, 1);
      }
    }

    auto eCamera = cameraRef.get(registry);

    if ( eCamera == entt::null )
      cameraRef = null_id;

    const bool viewportIsOpen = ImGui::Begin(
      windowTitle.c_str(), &windowOpened,
      windowFlags );

    if ( viewportIsOpen == false ||
         windowOpened == false )
    {
      iter->visible = false;

      if ( windowOpened == false )
        windowsToClose.push_back(viewportIndex);

      ImGui::End(); // windowTitle
      continue;
    }

    iter->visible = true;

    const auto windowRegion = ImGui::GetContentRegionAvail();

    if ( windowRegion.x <= 0.0f ||
         windowRegion.y <= 0.0f )
    {
      ImGui::End(); // windowTitle
      continue;
    }

    const auto viewportPos = ImGui::GetCurrentWindowRead()->InnerRect.Min;
    auto viewportSize = ImGui::GetCurrentWindowRead()->InnerRect.Max;
    viewportSize.x -= viewportPos.x;
    viewportSize.y -= viewportPos.y;

    iter->pos = {viewportPos.x, viewportPos.y};

    if ( iter->framebuffer.update({viewportSize.x, viewportSize.y}) == true )
    {
      auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
      readbackQueue.clear();
    }

    if ( windowsToClose.empty() == true )
      ImGui::GetBackgroundDrawList()->AddImage(
        (ImTextureID) iter->framebuffer.textureAlbedo.id(),
        {viewportPos.x, viewportPos.y},
        {viewportPos.x + viewportSize.x, viewportPos.y + viewportSize.y},
        ImVec2{0, 1}, ImVec2{1, 0} );

    const float comboWidth =
      std::min( ImGui::GetContentRegionAvail().x,
                ImGui::CalcTextSize("camera_id_long_enough").x
                + ImGui::GetFrameHeightWithSpacing());

    ImGui::SetNextItemWidth(comboWidth);

    if ( ImGui::BeginCombo("##cameraIdCombo",
                           cameraRef.id.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      mCameraFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto&& [entity, cCamera, cTag]
              : registry.view <Camera, Tag> ().each() )
      {
        if ( mCameraFilter.query(cTag.id.str()) == false )
          continue;

        const bool selected = cameraRef.id == cTag.id;

        if ( selected == true ||
             hasViewport(cTag.id) == true )
          continue;

        if ( ImGui::Selectable(cTag.id.str().c_str(), selected) )
        {
          cameraRef = cTag;
          eCamera = entity;
        }
      }

      ImGui::EndCombo(); // cameraIdCombo
    }

    if ( eCamera != entt::null &&
         registry.all_of <Camera, Transform, SceneNode> (eCamera) == true )
    {
      const auto& [cCamera, cCameraTransform] =
        registry.get <Camera, Transform> (eCamera);

      auto camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
      const auto camProjection = cCamera.projMatrix({viewportSize.x, viewportSize.y});

      const auto camViewport = cCamera.viewportScaled({viewportSize.x, viewportSize.y});

      ImGuizmo::SetID(viewportIndex);
      ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
      ImGuizmo::SetOrthographic(cCamera.projectionType == Camera::Projection::Orthographic);

      ImGuizmo::SetRect(
        viewportPos.x + camViewport.x,
        viewportPos.y + viewportSize.y - camViewport.w - camViewport.y,
        camViewport.z,
        camViewport.w );

      const auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

      const auto selectedEntities = entityManagerUi.selectedEntities();
      const auto selectedEntity = selectedEntities.empty() == false
                                ? *selectedEntities.begin()
                                : entt::null ;

      bool viewCubeHovered {};

      if ( entity_valid(selectedEntity, registry) == true &&
           selectedEntity != eCamera &&
           (viewCubeHovered == false ||
            mGizmoCubeUsingIndex == -1 ||
            ImGuizmo::IsUsing() == true) )
      {
        auto cTransform = registry.try_get <Transform> (selectedEntity);

        if ( cTransform != nullptr )
        {
          auto matrixWorld = GetWorldMatrix(registry, selectedEntity, *cTransform);

          glm::mat4 matrixDelta {};

          if ( ImGuizmo::Manipulate(
            glm::value_ptr(camView),
            glm::value_ptr(camProjection),
            mGizmoOperation,
            mGizmoSpace,
            glm::value_ptr(matrixWorld),
            glm::value_ptr(matrixDelta),
            mGizmoSnapEnabled ? glm::value_ptr(mGizmoSnap) : nullptr) )
          {
//            Guizmo bug workaround
            const auto entityPosRelative =
              glm::translate(camView, glm::vec3{matrixWorld[3]})[3];

            if ( entityPosRelative.z < -cCamera.zRange.first &&
                 entityPosRelative.z > -cCamera.zRange.second )
            {
              matrixWorld = ToLocalSpace(
                matrixWorld, registry,
                selectedEntity, *cTransform );

              glm::vec3 nodeTranslation {};
              glm::quat nodeOrientation {};
              glm::vec3 nodeScale {};
              glm::vec3 skew {};
              glm::vec4 perspective {};

              glm::decompose(
                matrixWorld,
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

      if ( mGizmoSettingsExpanded == true )
      {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));

        if ( ImGui::BeginChild("##gizmoSettings",
                               {comboWidth, 0},
                               true) )
        {
          ImGui::AlignTextToFramePadding();
//          ImGui::Text("%s", format("Z-buffer depth: {}",
//                                   cCamera.zBuffer.size()).c_str());

          if ( ImGui::CollapsingHeader("View cube", ImGuiTreeNodeFlags_DefaultOpen) )
          {
            const auto gizmoSize = comboWidth * 0.5f;

            const auto cursorPos = ImGui::GetMousePos();
            const auto rectPos = ImGui::GetCursorScreenPos();

            viewCubeHovered = pointInRect({cursorPos.x, cursorPos.y},
                                          {rectPos.x - 1, rectPos.x + gizmoSize + 1,
                                          rectPos.y - 1, rectPos.y + gizmoSize + 1});

            if ( ImGui::BeginChild("##viewCube", {gizmoSize, gizmoSize}, true) )
            {
              if ( ImGuizmo::IsUsing() == false ||
                   viewCubeHovered == false )
              {
                const auto camViewPrev = camView;

                ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

                ImGuizmo::ViewManipulate(
                  glm::value_ptr(camView),
                  mViewCubeOrbitRadius,
                  ImGui::GetWindowPos(),
                  ImGui::GetWindowSize(),
                  ImGui::GetColorU32(ImGuiCol_WindowBg) );

                if ( camView != camViewPrev &&
                     (mGizmoCubeUsingIndex == viewportIndex ||
                      mGizmoCubeUsingIndex == -1) &&
                     registry.all_of <CasqadiumEditorInternal> (eCamera) == true )
                {
                  camView = glm::inverse(camView);

                  glm::vec3 camTranslation = camView[3];
                  auto camOrientation = glm::toQuat(camView);

                  if ( glm::all(glm::isfinite(camView[3])) == true &&
                       glm::all(glm::isfinite(glm::eulerAngles(camOrientation))) == true )
                  {
                    camOrientation = ToLocalSpace(camOrientation, registry,
                                                  eCamera, cCameraTransform);

                    if ( mViewCubeOrbitEnabled == true )
                    {
                      camTranslation = ToLocalSpace(camTranslation, registry,
                                                    eCamera, cCameraTransform);
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
            ImGui::EndChild(); // ##viewCube

            ImGui::Checkbox("Orbit mode", &mViewCubeOrbitEnabled);

            if ( mViewCubeOrbitEnabled == true )
            {
              ImGui::DragFloat("##cubeOrbitRadius", &mViewCubeOrbitRadius, 0.1f,
                               glm::epsilon <float> (),
                               std::numeric_limits <float>::max(),
                               "Radius: %.1f");
            }
          }

          if ( ImGui::CollapsingHeader("Gizmo space", ImGuiTreeNodeFlags_DefaultOpen) )
          {
            if ( ImGui::RadioButton("Local", mGizmoSpace == ImGuizmo::LOCAL) )
              mGizmoSpace = ImGuizmo::LOCAL;

            if ( mGizmoOperation != ImGuizmo::SCALE )
              if ( ImGui::RadioButton("World", mGizmoSpace == ImGuizmo::WORLD) )
                mGizmoSpace = ImGuizmo::WORLD;
          }

          if ( ImGui::CollapsingHeader("Gizmo operation", ImGuiTreeNodeFlags_DefaultOpen) )
          {
            if ( ImGui::RadioButton("Translate",
                                    mGizmoOperation == ImGuizmo::TRANSLATE) )
              mGizmoOperation = ImGuizmo::TRANSLATE;

            if ( ImGui::RadioButton("Rotate",
                                    mGizmoOperation == ImGuizmo::ROTATE) )
              mGizmoOperation = ImGuizmo::ROTATE;

            if ( ImGui::RadioButton("Scale",
                                    mGizmoOperation == ImGuizmo::SCALE) )
            {
              mGizmoSpace = ImGuizmo::LOCAL;
              mGizmoOperation = ImGuizmo::SCALE;
            }
          }

          if ( ImGui::CollapsingHeader("Gizmo snap", ImGuiTreeNodeFlags_DefaultOpen) )
          {
            switch (mGizmoOperation)
            {
              case ImGuizmo::TRANSLATE:
                ImGui::Checkbox("XYZ snap##gizmoSnap", &mGizmoSnapEnabled);

                if ( mGizmoSnapEnabled == true )
                {
                  ImGui::DragFloat("##snapX", &mGizmoSnap[0], 1.0f, 0.0f,
                                    std::numeric_limits <float>::max());
                  ImGui::DragFloat("##snapY", &mGizmoSnap[1], 1.0f, 0.0f,
                                    std::numeric_limits <float>::max());
                  ImGui::DragFloat("##snapZ", &mGizmoSnap[2], 1.0f, 0.0f,
                                    std::numeric_limits <float>::max());
                }
                break;

              case ImGuizmo::ROTATE:
                ImGui::Checkbox("Angle snap##gizmoSnap", &mGizmoSnapEnabled);

                if ( mGizmoSnapEnabled == true )
                  ImGui::DragFloat("##angleSnap", glm::value_ptr(mGizmoSnap),
                                   1.0f, 0.0f,
                                   std::numeric_limits <float>::max());
                break;

              case ImGuizmo::SCALE:
                ImGui::Checkbox("Scale snap##gizmoSnap", &mGizmoSnapEnabled);

                if ( mGizmoSnapEnabled == true )
                  ImGui::DragFloat("##scaleSnap", glm::value_ptr(mGizmoSnap),
                                   1.0f, 0.0f,
                                   std::numeric_limits <float>::max());
                break;

              default:
                break;
            }
          }
        }

        ImGui::EndChild(); // gizmoSettings

        ImGui::PopStyleColor(); // ImGuiCol_ChildBg

        ImGui::SameLine();

        if ( ImGui::Button("<") )
          mGizmoSettingsExpanded = false;

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip("Collapse");
      }
      else
      {
        mGizmoSettingsExpanded = ImGui::Button(">");

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip("Expand");
      }
    }

    ImGui::End(); // windowTitle
  }

  std::reverse(windowsToClose.begin(), windowsToClose.end());

  for ( const auto windowToClose : windowsToClose )
    mViewports.erase(mViewports.begin() + windowToClose);
}

} // namespace cqde::ui
