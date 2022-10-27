#include <cqde/types/physics/PhysicsEventListener.hpp>

#include <cqde/components/physics/CollisionBody.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/RigidBody.h>


namespace cqde::types
{

entt::entity
PhysicsEventListener::bodyToEntity(
  const rp3d::CollisionBody* body ) const
{
  if ( body == nullptr )
    return entt::null;

  if ( dynamic_cast <const rp3d::RigidBody*> (body) != nullptr )
  {
    const auto bodyInstance
      = static_cast <const compos::RigidBody*> (body->getUserData());

    return entt::to_entity(mRegistry, *bodyInstance);
  }

  const auto bodyInstance
    = static_cast <const compos::CollisionBody*> (body->getUserData());

  return entt::to_entity(mRegistry, *bodyInstance);
}

PhysicsEventListener::PhysicsEventListener(
  entt::registry& registry )
  : mRegistry{registry}
{}

void
PhysicsEventListener::onContact(
  const CollisionCallback::CallbackData& data )
{
  using EventType = CollisionCallback::ContactPair::EventType;

  for ( int i = 0; i < data.getNbContactPairs(); ++i )
  {
    const auto pair = data.getContactPair(i);

    const auto eBody1 = bodyToEntity(pair.getBody1());
    const auto eBody2 = bodyToEntity(pair.getBody2());

    const auto collider1 = static_cast <Collider*> (pair.getCollider1()->getUserData());
    const auto collider2 = static_cast <Collider*> (pair.getCollider2()->getUserData());

    const auto& callbackManager = mRegistry.ctx().at <CallbackManager> ();

    switch (pair.getEventType())
    {
      case EventType::ContactStart:
      {
        collider1->onEnter(mRegistry, {eBody1, eBody2});
        collider2->onEnter(mRegistry, {eBody2, eBody1});
        break;
      }
      case EventType::ContactStay:
      {
        collider1->onStay(mRegistry, {eBody1, eBody2});
        collider2->onStay(mRegistry, {eBody2, eBody1});
        break;
      }
      case EventType::ContactExit:
      {
        collider1->onLeave(mRegistry, {eBody1, eBody2});
        collider2->onLeave(mRegistry, {eBody2, eBody1});
        break;
      }
    }
  }
}

void
PhysicsEventListener::onTrigger(
  const OverlapCallback::CallbackData& data )
{
  using EventType = OverlapCallback::OverlapPair::EventType;

  for ( int i = 0; i < data.getNbOverlappingPairs(); ++i )
  {
    const auto pair = data.getOverlappingPair(i);

    const auto eBody1 = bodyToEntity(pair.getBody1());
    const auto eBody2 = bodyToEntity(pair.getBody2());

    const auto collider1 = static_cast <Collider*> (pair.getCollider1()->getUserData());
    const auto collider2 = static_cast <Collider*> (pair.getCollider2()->getUserData());

    const auto& callbackManager = mRegistry.ctx().at <CallbackManager> ();

    switch (pair.getEventType())
    {
      case EventType::OverlapStart:
      {
        collider1->onEnter(mRegistry, {eBody1, eBody2});
        collider2->onEnter(mRegistry, {eBody2, eBody1});
        break;
      }
      case EventType::OverlapStay:
      {
        collider1->onStay(mRegistry, {eBody1, eBody2});
        collider2->onStay(mRegistry, {eBody2, eBody1});

        break;
      }
      case EventType::OverlapExit:
      {
        collider1->onLeave(mRegistry, {eBody1, eBody2});
        collider2->onLeave(mRegistry, {eBody2, eBody1});

        break;
      }
    }
  }
}

} // namespace cqde::types
