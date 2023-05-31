#include <cqde/callbacks/input.hpp>

#include <cqde/types/TickCurrent.hpp>

#include <cqde/components/Transform.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/InputController.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>


namespace cqde::callbacks
{

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.right() * value;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.up() * value;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  cTransform.translation += cTransform.front() * value;

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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(value),
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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(value),
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
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(value),
                                    glm::vec3 {0.0f, 0.0f, 1.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

} // namespace cqde::callbacks
