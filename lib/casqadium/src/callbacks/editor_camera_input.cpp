#include <cqde/callbacks/editor_camera_input.hpp>
#include <cqde/callbacks/entities.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/CasqadiumEditorCameraSettings.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>


namespace cqde::callbacks
{

void
editorCameraControlOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using ui::ViewportManagerUi;
  using namespace compos;

  for ( const auto&& [eCamera, cTag, cCamera]
          : registry.view <Tag, Camera, CasqadiumEditorInternal> ().each() )
  {
    if ( registry.ctx().at <ViewportManagerUi> ().mouseOverViewport(cTag.id) == false )
      continue;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      continue;

    entityInputOn(registry, {eCamera});

    registry.emplace_or_replace <WantsMouseCentered> (eCamera);
    registry.emplace_or_replace <WantsMouseHidden> (eCamera);

    return;
  }
};

void
editorCameraControlOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using namespace compos;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));

  entityInputOff(registry, args);

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.removeLater(eCamera, entityManager.componentType <WantsMouseCentered> ());
  entityManager.removeLater(eCamera, entityManager.componentType <WantsMouseHidden> ());
};

void
editorCameraFovControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using types::ControlAxis;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cCamera.fov = axis->value;
};

void
editorCameraZoomControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using types::ControlAxis;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Orthographic )
    cCamera.zoom = axis->value;
};

void
editorCameraSpeedControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cSettings = registry.get <CasqadiumEditorCameraSettings> (entity);
  cSettings.speed += axis->value * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.right() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.up() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

void
editorCameraTranslateZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::Transform;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto&& [cTransform, cCamera, cSettings] = registry.get <Transform, Camera, CasqadiumEditorCameraSettings> (entity);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cTransform.translation += cTransform.front() * axis->value * cSettings.speed * dt;

  axis->value = 0.0f;
};

} // namespace cqde::callbacks
