#include <cqde/callbacks/editor_gizmo_input.hpp>

#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
editorGizmoSetModeTranslate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
}

void
editorGizmoSetModeRotate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::ROTATE);
}

void
editorGizmoSetModeScale(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoOperation(ImGuizmo::OPERATION::SCALE);
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::LOCAL);
}

void
editorGizmoSetSpaceLocal(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::LOCAL);
}

void
editorGizmoSetSpaceWorld(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();
  viewportManagerUi.setGizmoSpace(ImGuizmo::MODE::WORLD);
}

} // namespace cqde::callbacks
