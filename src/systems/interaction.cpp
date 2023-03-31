#include <cqde/systems/render.hpp>

#include <cqde/assert.hpp>
#include <cqde/math_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/graphics/FrameReadback.hpp>
#include <cqde/types/graphics/PrimaryRenderTarget.hpp>

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

  using types::CasqadiumEngine;
  using types::FrameReadbackQueue;
  using types::FrameReadbackResult;
  using types::PrimaryRenderTarget;
  using CallbackArgs = types::CallbackManager::CallbackArgs;

  const auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  auto& mainTarget = registry.ctx().get <PrimaryRenderTarget> ();

  if ( mainTarget.target.fbo == 0 )
    return;

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();

  for ( const auto&& [eCamera, cCamera, cInteractionSource]
          : registry.view <Camera, InteractionSource, SubscriberUpdate> ().each() )
  {
    glm::vec2 probePos {};

    switch (cInteractionSource.type)
    {
      case InteractionType::ViewportCenter:
      {
        probePos = rectCenter(cCamera.viewportScaled(engine.windowSize()));
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

      default:
        CQDE_ASSERT_DEBUG(false, continue);
    }

    readbackQueue.push( mainTarget.target.fbo, probePos, {1u, 1u},
    [eCamera = eCamera]
    ( entt::registry& registry,
      const CallbackArgs& args )
    {
      if ( registry.all_of <Camera, InteractionSource, SubscriberUpdate> (eCamera) == false )
        return;

      auto& cInteractionSource = registry.get <InteractionSource> (eCamera);

      cInteractionSource.listener = entt::null;

      const auto result =
        std::any_cast <FrameReadbackResult> (args.at(0));

      const auto entity = static_cast <entt::entity> (result.data.front());

      if ( entity == entt::null )
        return;

      if ( registry.all_of <InteractionListener, SubscriberUpdate> (entity) == false )
        return;

      auto& cInteractionListener = registry.get <const InteractionListener> (entity);

      bool actionFound {};

      for ( const auto& actionId : cInteractionSource.actions )
        if ( cInteractionListener.actions.count(actionId) > 0 )
        {
          actionFound = true;
          break;
        }

      if ( actionFound == false )
        return;

      if ( cInteractionSource.radius > 0.0f )
      {
        auto& cSourceTransform = registry.get <Transform> (eCamera);
        auto& cListenerTransform = registry.get <Transform> (entity);

        const auto sourcePos = GetWorldMatrix(registry, eCamera, cSourceTransform)[3];
        const auto listenerPos = GetWorldMatrix(registry, entity, cListenerTransform)[3];

        const auto distance = glm::length(listenerPos - sourcePos);

        if ( distance > cInteractionSource.radius )
          return;
      }

      cInteractionSource.listener = entity;
    });
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
