#include <cqde/callbacks/input.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/components/Transform.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/InputController.hpp>

#include <entt/entity/registry.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>


namespace cqde::callbacks
{

void
mouseAutoCenterEnable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetKeepMouseCentered(true);
};

void
mouseAutoCenterDisable(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetKeepMouseCentered(false);
};

void
mouseAutoCenterToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  if ( olc::platform->ptrPGE->GetKeepMouseCentered() == true )
    return olc::platform->ptrPGE->SetKeepMouseCentered(false);

  olc::platform->ptrPGE->SetKeepMouseCentered(true);
};

void
mouseCursorHide(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});
};

void
mouseCursorShow(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  olc::platform->ptrPGE->ResetMouseCursor();
};

void
mouseGrabToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::InputController;
  using compos::SubscriberInput;

  if ( olc::platform->ptrPGE->GetKeepMouseCentered() == true )
  {
    mouseAutoCenterDisable(registry, args);
    mouseCursorShow(registry, args);

    return;
  }

  mouseAutoCenterEnable(registry, args);
  mouseCursorHide(registry, args);
}

void
controlTranslateXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.right() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlTranslateYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.up() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlTranslateZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.front() * axis->value * dt;

  axis->value = 0.0f;
};

void
controlPitchRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {1.0f, 0.0f, 0.0f});

  const auto orientationPrev = cTransform.orientation;

  cTransform.orientation = glm::normalize(cTransform.orientation * angle);

  if ( cTransform.up().y < 0.0f )
    cTransform.orientation = orientationPrev;

  axis->value = 0.0f;
};

void
controlYawRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {0.0f, 1.0f, 0.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

void
controlRollRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::Transform;
  using types::ControlAxis;
  using types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto& tick = registry.ctx().at <TickCurrent> ();
  const auto ticks = tick.ticksElapsed;
  const auto elapsed = tick.tickInterval;

  const float dt = ticks * static_cast <double> (elapsed);

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {0.0f, 0.0f, 1.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

} // namespace cqde::callbacks
