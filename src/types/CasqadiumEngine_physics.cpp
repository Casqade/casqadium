#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/PhysicsManager.hpp>

#include <cqde/types/physics/ColliderFactory.hpp>
#include <cqde/types/physics/colliders/ColliderBox.hpp>
#include <cqde/types/physics/colliders/ColliderCapsule.hpp>
#include <cqde/types/physics/colliders/ColliderHeightField.hpp>
#include <cqde/types/physics/colliders/ColliderSphere.hpp>


namespace cqde::types
{

void
CasqadiumEngine::initPhysics()
{
  mRegistry.ctx().emplace <PhysicsManager> (mRegistry);

  auto& colliderShapeFactory = mRegistry.ctx().emplace <ColliderFactory> ();

  colliderShapeFactory.registerCollider <ColliderBox> ("BoxShape");
  colliderShapeFactory.registerCollider <ColliderCapsule> ("CapsuleShape");
  colliderShapeFactory.registerCollider <ColliderHeightField> ("HeightFieldShape");
  colliderShapeFactory.registerCollider <ColliderSphere> ("SphereShape");
}

} // namespace cqde::types
