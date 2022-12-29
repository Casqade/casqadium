#include <cqde/audio_helpers.hpp>

#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/audio/Audio3dParams.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/RigidBody.h>

#include <soloud.h>


namespace cqde
{

AudioListenerParameters
calcAudioListenerParams(
  entt::registry& registry,
  const entt::entity eListener,
  const compos::Transform& cListenerTransform )
{
  using compos::Camera;
  using compos::RigidBody;

  const auto [cListenerBody, cListenerCamera]
    = registry.try_get <const RigidBody, const Camera> (eListener);

  glm::mat4 listenerTransform {};

  if ( cListenerCamera != nullptr )
    listenerTransform = cListenerCamera->viewMatrix(registry, eListener, cListenerTransform);
  else
    listenerTransform = GetWorldMatrix(registry, eListener, cListenerTransform );

  glm::vec3 listenerVelocity {};

  if ( cListenerBody != nullptr )
    listenerVelocity = ToLocalSpace(
      rp3dToGlm(cListenerBody->body->getLinearVelocity()),
      registry,
      eListener,
      cListenerTransform );

  return
  {
    listenerTransform,
    listenerVelocity,
  };
}

void
updateAudio3dParams(
  entt::registry& registry,
  const AudioHandle handle,
  const AudioListenerParameters& listenerParams,
  const entt::entity eListener,
  const compos::Transform& cListenerTransform,
  const entt::entity eSource,
  const compos::Transform& cSourceTransform )
{
  using compos::Camera;
  using compos::Transform;
  using compos::RigidBody;
  using compos::Audio3dParams;
  using compos::AudioListener3d;
  using compos::SubscriberUpdate;
  using types::EntityManager;

  auto& soloud = registry.ctx().at <SoLoud::Soloud> ();

  const auto sourceTransform = GetWorldMatrix(registry, eSource, cSourceTransform );
  const auto sourceToListenerTransform = listenerParams.transform * sourceTransform;

  AudioSourceParams params
  {
    sourceToListenerTransform[3],
    listenerParams.velocity,
  };

  const auto cSourceBody = registry.try_get <const RigidBody> (eSource);

  if ( cSourceBody != nullptr )
  {
    const auto sourceVelocity = rp3dToGlm(cSourceBody->body->getLinearVelocity());
    params.velocity -= ToLocalSpace(sourceVelocity, registry, eListener, cListenerTransform);
  }

  soloud.set3dSourceParameters(
    handle,
    params.pos.x,
    params.pos.y,
    params.pos.z,
    params.velocity.x,
    params.velocity.y,
    params.velocity.z );

  auto audioParams = registry.try_get <const Audio3dParams> (eSource);

  if ( audioParams == nullptr )
    return;

  soloud.set3dSourceAttenuation(
    handle,
    audioParams->attenuation.model,
    audioParams->attenuation.rolloff );

  soloud.set3dSourceMinMaxDistance(
    handle,
    audioParams->attenuation.distance.min,
    audioParams->attenuation.distance.max );

  soloud.set3dSourceDopplerFactor(
    handle,
    audioParams->dopplerFactor );
}

} // namespace cqde
