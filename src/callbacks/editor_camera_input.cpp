#include <cqde/callbacks/editor_camera_input.hpp>
#include <cqde/callbacks/common_routine.hpp>

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
#include <cqde/components/WantsMouseGrabbed.hpp>
#include <cqde/components/WantsCursorOverridden.hpp>


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
    if ( registry.ctx().get <ViewportManagerUi> ().mouseOverViewport(cTag.id) == false )
      continue;

    if ( registry.all_of <SubscriberInput> (eCamera) == true )
      continue;

    entityInputOn(registry, {eCamera});

    registry.emplace_or_replace <WantsMouseGrabbed> (eCamera);

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

  auto& entityManager = registry.ctx().get <EntityManager> ();
  entityManager.removeLater(eCamera, entityManager.componentType <WantsMouseGrabbed> ());
};

void
editorCameraFovControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cCamera = registry.get <Camera> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cCamera.fov += value;

  axis->value = 0.0f;
};

void
editorCameraZoomControl(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Camera;
  using compos::CasqadiumEditorCameraSettings;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto eCamera = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cCamera, cSettings] = registry.get <Camera, CasqadiumEditorCameraSettings> (eCamera);

  if ( cCamera.projectionType == Camera::Projection::Orthographic )
    cCamera.scale += value * cSettings.speed;

  axis->value = 0.0f;
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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cSettings = registry.get <CasqadiumEditorCameraSettings> (entity);
  cSettings.speed += value;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.right() * value * cSettings.speed;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cSettings] = registry.get <Transform, CasqadiumEditorCameraSettings> (entity);

  cTransform.translation += cTransform.up() * value * cSettings.speed;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cCamera, cSettings] = registry.get <Transform, Camera, CasqadiumEditorCameraSettings> (entity);

  if ( cCamera.projectionType == Camera::Projection::Perspective )
    cTransform.translation += cTransform.front() * value * cSettings.speed;

  axis->value = 0.0f;
};

} // namespace cqde::callbacks
