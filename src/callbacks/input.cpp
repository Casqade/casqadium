#include <cqde/callbacks/input.hpp>

#include <cqde/types/CasqadiumEngine.hpp>

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

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
  using types::CasqadiumEngine;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();
  const float dt = static_cast <double> (engine->deltaTime());

  auto& cTransform = registry.get <Transform> (entity);

  const auto angle = glm::angleAxis(glm::radians(axis->value * dt),
                                    glm::vec3 {0.0f, 0.0f, 1.0f});

  cTransform.orientation = glm::normalize(angle * cTransform.orientation);

  axis->value = 0.0f;
};

} // namespace cqde::callbacks
