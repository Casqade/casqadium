#include <cqde/audio_helpers.hpp>

#include <cqde/conversion/rp3d_glm.hpp>

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
  using compos::RigidBody;

  const auto cListenerBody =
    registry.try_get <const RigidBody> (eListener);

  const auto listenerTransform = GetWorldMatrix(
    registry, eListener, cListenerTransform );

  glm::vec3 listenerVelocity {};

  if ( cListenerBody != nullptr )
    listenerVelocity = glm::translate(
      glm::mat4{glm::inverse(glm::mat3{listenerTransform})},
      rp3dToGlm(cListenerBody->body->getLinearVelocity()) )[3];

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
  using compos::RigidBody;
  using compos::Audio3dParams;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  const auto sourceToListenerTransform = GetDeltaMatrix(
    listenerParams.transform,
    GetWorldMatrix(registry, eSource, cSourceTransform ) );

  AudioSourceParams params
  {
    sourceToListenerTransform[3],
    listenerParams.velocity,
  };

  const auto cSourceBody = registry.try_get <const RigidBody> (eSource);

  if ( cSourceBody != nullptr )
  {
    const glm::vec3 sourceVelocity = glm::translate(
      glm::mat4{glm::inverse(glm::mat3{listenerParams.transform})},
      rp3dToGlm(cSourceBody->body->getLinearVelocity()) )[3];

    params.velocity = sourceVelocity - params.velocity;
  }

  soloud.set3dSourceParameters(
    handle,
    params.pos.x,
    params.pos.y,
    params.pos.z,
    -params.velocity.x,
    -params.velocity.y,
    -params.velocity.z );

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
