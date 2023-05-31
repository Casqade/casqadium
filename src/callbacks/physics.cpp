#include <cqde/callbacks/physics.hpp>

#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/components/Transform.hpp>
#include <cqde/components/physics/RigidBody.hpp>
#include <cqde/components/physics/ForceEmitter.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/vector_query.hpp>
#include <glm/gtx/compatibility.hpp>

#include <reactphysics3d/body/RigidBody.h>


namespace cqde::callbacks
{

void
forceEmitterCallback(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::RigidBody;
  using compos::Transform;
  using compos::ForceEmitter;

  const auto eEmitter = std::any_cast <entt::entity> (args.at(0));
  const auto eBody = std::any_cast <entt::entity> (args.at(1));

  const auto [cTransform1, cEmitter] = registry.try_get <Transform, ForceEmitter> (eEmitter);
  const auto [cTransform2, cBody] = registry.try_get <Transform, RigidBody> (eBody);

  if ( cEmitter == nullptr ||
       cBody == nullptr ||
       cTransform1 == nullptr ||
       cTransform2 == nullptr )
    return;

  glm::vec3 direction {};

  if ( glm::isNull(glm::vec3{cEmitter->force}, glm::epsilon <float> ()) == true )
  {
    const glm::vec3 emitterPos = GetWorldMatrix(registry, eEmitter, *cTransform1)[3];
    const glm::vec3 bodyPos = GetWorldMatrix(registry, eBody, *cTransform2)[3];

    direction = glm::normalize(emitterPos - bodyPos);
  }
  else if ( cEmitter->useWorldSpace == true )
    direction = glm::vec{cEmitter->force};

  else
    direction = ToWorldSpace(glm::vec3{cEmitter->force},
                             registry,
                             eEmitter,
                             *cTransform1);

  const auto force = direction * cEmitter->force.w;

  if ( glm::all(glm::isfinite(force)) == true )
    cBody->body->applyWorldForceAtCenterOfMass(glmToRp3d(force));
}

void
gravityEmitterCallback(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::RigidBody;
  using compos::Transform;
  using compos::ForceEmitter;

  const auto eEmitter = std::any_cast <entt::entity> (args.at(0));
  const auto eBody = std::any_cast <entt::entity> (args.at(1));

  const auto [cTransform1, cEmitter] = registry.try_get <Transform, ForceEmitter> (eEmitter);
  const auto [cTransform2, cBody] = registry.try_get <Transform, RigidBody> (eBody);

  if ( cEmitter == nullptr ||
       cBody == nullptr ||
       cTransform1 == nullptr ||
       cTransform2 == nullptr )
    return;

  glm::vec3 direction {};

  if ( glm::isNull(glm::vec3{cEmitter->force}, glm::epsilon <float> ()) == true )
  {
    const glm::vec3 emitterPos = GetWorldMatrix(registry, eEmitter, *cTransform1)[3];
    const glm::vec3 bodyPos = GetWorldMatrix(registry, eBody, *cTransform2)[3];

    direction = glm::normalize(emitterPos - bodyPos);
  }
  else if ( cEmitter->useWorldSpace == true )
    direction = glm::vec{cEmitter->force};

  else
    direction = ToWorldSpace(glm::vec3{cEmitter->force},
                             registry,
                             eEmitter,
                             *cTransform1);

  const auto force = direction * cEmitter->force.w * cBody->body->getMass();

  if ( glm::all(glm::isfinite(force)) == true )
    cBody->body->applyWorldForceAtCenterOfMass(glmToRp3d(force));
}


void
forceXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalForceAtCenterOfMass({value, 0.0f, 0.0f});

  axis->value = 0.0f;
}

void
forceYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalForceAtCenterOfMass({0.0f, value, 0.0f});

  axis->value = 0.0f;
}

void
forceZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalForceAtCenterOfMass({0.0f, 0.0f, value});

  axis->value = 0.0f;
}


void
torqueXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalTorque({value, 0.0f, 0.0f});

  axis->value = 0.0f;
}

void
torqueYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalTorque({0.0f, value, 0.0f});

  axis->value = 0.0f;
}

void
torqueZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  cRigidBody.body->applyLocalTorque({0.0f, 0.0f, value});

  axis->value = 0.0f;
}

void
speedLinearXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getLinearVelocity();
  velocity.x += value;
  cRigidBody.body->setLinearVelocity(velocity);

  axis->value = 0.0f;
}

void
speedLinearYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getLinearVelocity();
  velocity.y += value;
  cRigidBody.body->setLinearVelocity(velocity);

  axis->value = 0.0f;
}

void
speedLinearZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getLinearVelocity();
  velocity.z += value;
  cRigidBody.body->setLinearVelocity(velocity);

  axis->value = 0.0f;
}


void
speedLinearLocalXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::Transform;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cRigidBody] = registry.get <Transform, RigidBody> (entity);

  const auto velocity = cRigidBody.body->getLinearVelocity();

  const auto velocityDelta = ToWorldSpace(
    cTransform.right() * value,
    registry, entity,
    cTransform );

  cRigidBody.body->setLinearVelocity(velocity + glmToRp3d(velocityDelta));

  axis->value = 0.0f;
}

void
speedLinearLocalYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::Transform;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cRigidBody] = registry.get <Transform, RigidBody> (entity);

  const auto velocity = cRigidBody.body->getLinearVelocity();

  const auto velocityDelta = ToWorldSpace(
    cTransform.up() * value,
    registry, entity,
    cTransform );

  cRigidBody.body->setLinearVelocity(velocity + glmToRp3d(velocityDelta));

  axis->value = 0.0f;
}

void
speedLinearLocalZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::Transform;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto&& [cTransform, cRigidBody] = registry.get <Transform, RigidBody> (entity);

  const auto velocity = cRigidBody.body->getLinearVelocity();

  const auto velocityDelta = ToWorldSpace(
    cTransform.front() * value,
    registry, entity,
    cTransform );

  cRigidBody.body->setLinearVelocity(velocity + glmToRp3d(velocityDelta));

  axis->value = 0.0f;
}


void
speedAngularXRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getAngularVelocity();
  velocity.x += value;
  cRigidBody.body->setAngularVelocity(velocity);

  axis->value = 0.0f;
}

void
speedAngularYRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getAngularVelocity();
  velocity.y += value;
  cRigidBody.body->setAngularVelocity(velocity);

  axis->value = 0.0f;
}

void
speedAngularZRelative(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;
  using cqde::compos::RigidBody;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));
  const auto isEventComplete = std::any_cast <bool> (args.at(3));

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = isEventComplete ? 1.0 : static_cast <double> (tick.tickInterval);
  const float value = axis->value * dt;

  auto& cRigidBody = registry.get <RigidBody> (entity);

  auto velocity = cRigidBody.body->getAngularVelocity();
  velocity.z += value;
  cRigidBody.body->setAngularVelocity(velocity);

  axis->value = 0.0f;
}

} // namespace cqde::callbacks
