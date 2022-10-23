#include <cqde/types/physics/PhysicsEventListener.hpp>
#include <cqde/types/CallbackManager.hpp>

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
        for ( const auto& callback : collider1->callbacks.onEnter )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onEnter )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

        break;
      }
      case EventType::ContactStay:
      {
        for ( const auto& callback : collider1->callbacks.onStay )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onStay )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

        break;
      }
      case EventType::ContactExit:
      {
        for ( const auto& callback : collider1->callbacks.onLeave )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onLeave )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

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
        for ( const auto& callback : collider1->callbacks.onEnter )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onEnter )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

        break;
      }
      case EventType::OverlapStay:
      {
        for ( const auto& callback : collider1->callbacks.onStay )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onStay )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

        break;
      }
      case EventType::OverlapExit:
      {
        for ( const auto& callback : collider1->callbacks.onLeave )
          callbackManager.execute(callback, mRegistry, {eBody1, eBody2});

        for ( const auto& callback : collider2->callbacks.onLeave )
          callbackManager.execute(callback, mRegistry, {eBody2, eBody1});

        break;
      }
    }
  }
}

} // namespace cqde::types
