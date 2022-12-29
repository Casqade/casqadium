#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/EntityReference.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

#include <vector>


namespace cqde::ui
{

struct Viewport
{
  types::EntityReference camera {};

  glm::vec4 viewport {};

  struct
  {
    ImVec2 pos {};
    ImVec2 size {};

  } window;


  Viewport() = default;
};

class ViewportManagerUi
{
  using EntityReference = types::EntityReference;

  std::vector <Viewport> mViewports {};

  StringFilter mCameraFilter {"Camera entity ID"};

  int32_t mGizmoCubeUsingIndex {-1};

  glm::vec3 mGizmoSnap {1.0f};

  ImGuizmo::OPERATION mGizmoOperation {ImGuizmo::TRANSLATE};
  ImGuizmo::MODE mGizmoSpace {ImGuizmo::LOCAL};

  float mViewCubeOrbitRadius {1.0f};

  bool  mGizmoSettingsExpanded {true};
  bool  mViewCubeOrbitEnabled {};
  bool  mGizmoSnapEnabled {};

  int32_t viewportIndex( const EntityId& cameraId ) const;

public:
  ViewportManagerUi() = default;

  void setGizmoOperation( const ImGuizmo::OPERATION );
  void setGizmoSpace( const ImGuizmo::MODE );

  std::vector <Viewport> viewports() const;

  ImGuizmo::OPERATION gizmoOperation() const;
  ImGuizmo::MODE gizmoCoordinateSpace() const;

  bool hasViewport( const EntityId& cameraId ) const;
  bool mouseOverViewport( const EntityId& cameraId ) const;

  void ui_show( entt::registry& );
  void ui_show_viewport_windows( entt::registry& );
};

} // namespace cqde::ui
