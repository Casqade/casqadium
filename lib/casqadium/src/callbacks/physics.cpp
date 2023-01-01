#include <cqde/callbacks/physics.hpp>

#include <cqde/conversion/rp3d_glm.hpp>

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

} // namespace cqde::callbacks