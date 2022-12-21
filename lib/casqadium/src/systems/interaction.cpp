#include <cqde/systems/render.hpp>

#include <cqde/math_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::systems
{

void
InteractionQuerySystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::InputController;
  using compos::InteractionListener;
  using compos::InteractionSource;
  using compos::SubscriberUpdate;
  using InteractionType = InteractionSource::Type;

  for ( const auto&& [eCamera, cCamera, cInteractionSource]
          : registry.view <Camera, InteractionSource, SubscriberUpdate> ().each() )
  {
    glm::vec2 probePos {};

    switch (cInteractionSource.type)
    {
      case InteractionType::ViewportCenter:
      {
        probePos = rectCenter(cCamera.viewportScaled());
        break;
      }

      case InteractionType::MousePos:
      {
        auto cInteractionController = registry.try_get <InputController> (eCamera);

        if ( cInteractionController == nullptr )
          continue;

        const auto cursorPosX = cInteractionController->axes.find("CursorPosX");
        const auto cursorPosY = cInteractionController->axes.find("CursorPosY");

        if ( cursorPosX == cInteractionController->axes.end() ||
             cursorPosY == cInteractionController->axes.end() )
          continue;

        probePos =
        {
          cursorPosX->second.value,
          cursorPosY->second.value,
        };
      }

      case InteractionType::PhysicsProbe:
        continue;
    }

    cInteractionSource.listener = entt::null;

    for ( auto& [vBuf, entity] : cCamera.zBuffer )
    {
      if ( registry.all_of <InteractionListener, SubscriberUpdate> (entity) == false )
        continue;

      if ( pointInRect( probePos, boundingBox(vBuf.vertices) ) == false )
        continue;

      auto& cInteractionListener = registry.get <const InteractionListener> (entity);

      bool actionFound {};

      for ( const auto& actionId : cInteractionSource.actions )
        if ( cInteractionListener.actions.count(actionId) > 0 )
        {
          actionFound = true;
          break;
        }

      if ( actionFound == false )
        continue;

      if ( cInteractionSource.radius > 0.0f )
      {
        auto& cSourceTransform = registry.get <Transform> (eCamera);
        auto& cListenerTransform = registry.get <Transform> (entity);

        const auto sourcePos = GetWorldMatrix(registry, eCamera, cSourceTransform)[3];
        const auto listenerPos = GetWorldMatrix(registry, entity, cListenerTransform)[3];

        const auto distance = glm::length(listenerPos - sourcePos);

        if ( distance > cInteractionSource.radius )
          continue;
      }

      cInteractionSource.listener = entity;
    }
  }
}

void
InteractionResetSystem(
  entt::registry& registry )
{
  using compos::InteractionSource;
  using compos::SubscriberUpdate;
  using InteractionType = InteractionSource::Type;

  for ( const auto&& [eInteractionSource, cInteractionSource]
          : registry.view <InteractionSource> ().each() )
    cInteractionSource.listener = entt::null;
}

} // namespace cqde::systems
