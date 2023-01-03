#include <cqde/systems/audio.hpp>

#include <cqde/audio_helpers.hpp>

#include <cqde/components/Transform.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/audio/AudioBus.hpp>
#include <cqde/components/audio/AudioDrivenTransform.hpp>
#include <cqde/components/audio/AudioListener3d.hpp>

#include <entt/entity/registry.hpp>

#include <soloud.h>


namespace cqde::systems
{

void
Audio3dSystem(
  entt::registry& registry )
{
  using compos::Transform;
  using compos::AudioListener3d;
  using compos::SubscriberUpdate;

  auto& soloud = registry.ctx().at <SoLoud::Soloud> ();

  for ( auto&& [eListener, cListener, cListenerTransform]
    : registry.view <AudioListener3d, const Transform, SubscriberUpdate> ().each() )
  {
    const auto listenerParams
      = calcAudioListenerParams(registry, eListener, cListenerTransform);

    std::vector <AudioHandle> instancesToRemove {};

    for ( const auto& [handle, eSource] : cListener.instances )
    {
      if ( soloud.isValidVoiceHandle(handle) == false &&
           soloud.isVoiceGroup(handle) == false )
      {
        instancesToRemove.push_back(handle);
        continue;
      }

      if ( eSource == entt::null )
      {
        instancesToRemove.push_back(handle);
        continue;
      }

      const auto cSourceTransform = registry.try_get <const Transform> (eSource);

      if ( cSourceTransform == nullptr )
      {
        instancesToRemove.push_back(handle);
        continue;
      }

      updateAudio3dParams(
        registry,
        handle,
        listenerParams,
        eListener,
        cListenerTransform,
        eSource,
        *cSourceTransform );
    }

    for ( const auto& instanceToRemove : instancesToRemove )
      cListener.instances.erase(instanceToRemove);
  }

  soloud.update3dAudio();
}

void
AudioDrivenTransformSystem(
  entt::registry& registry )
{
  using compos::Transform;
  using compos::SubscriberUpdate;
  using compos::AudioBus;
  using compos::AudioDrivenTransform;
  using types::TransformType;

  auto& soloud = registry.ctx().at <SoLoud::Soloud> ();

  for ( auto&& [eAudioTransform, cAudioTransform]
          : registry.view <AudioDrivenTransform> (entt::exclude <SubscriberUpdate>).each() )
    cAudioTransform.amplitude = {};

  for ( auto&& [eAudioTransform, cAudioTransform, cTransform]
          : registry.view <AudioDrivenTransform, Transform, SubscriberUpdate> ().each() )
  {
    const auto eSource = cAudioTransform.sourceEntity.get(registry);

    const auto transformSet =
    [&registry,
      entity = eAudioTransform,
      audioTransform = &cAudioTransform,
      transform = &cTransform]
    ( const float factor )
    {
      const auto axis = audioTransform->axis;

      switch (audioTransform->operation)
      {
        case TransformType::Translate:
        {
          auto translation = axis * factor;

          if ( audioTransform->useWorldSpace == true )
            translation = ToLocalSpace(translation, registry, entity, *transform);

          transform->translation =
          {
            glm::epsilonEqual(axis.x, 0.0f, glm::epsilon <float> () ) == false
              ? translation.x : transform->translation.x,
            glm::epsilonEqual(axis.y, 0.0f, glm::epsilon <float> () ) == false
              ? translation.y : transform->translation.y,
            glm::epsilonEqual(axis.z, 0.0f, glm::epsilon <float> () ) == false
              ? translation.z : transform->translation.z,
          };

          return;
        }

        case TransformType::Scale:
        {
          auto scale = axis * factor;

          if ( audioTransform->useWorldSpace == true )
            scale = ToLocalSpace(scale, registry, entity, *transform);

          transform->scale =
          {
            glm::epsilonEqual(axis.x, 0.0f, glm::epsilon <float> () ) == false
              ? scale.x : transform->scale.x,
            glm::epsilonEqual(axis.y, 0.0f, glm::epsilon <float> () ) == false
              ? scale.y : transform->scale.y,
            glm::epsilonEqual(axis.z, 0.0f, glm::epsilon <float> () ) == false
              ? scale.z : transform->scale.z,
          };

          return;
        }

        case TransformType::Rotate:
        {
          auto rotation = glm::angleAxis(factor, axis);

          if ( audioTransform->useWorldSpace == true )
            rotation = ToLocalSpace(rotation, registry, entity, *transform);

          transform->orientation = rotation;

          return;
        }
      }
    };

    if ( eSource == entt::null )
    {
      transformSet(cAudioTransform.offset);
      continue;
    }

    auto& cSourceBus = registry.get <const AudioBus> (eSource);

    const auto busHandle = cSourceBus.bus->mChannelHandle;

    if ( soloud.isValidVoiceHandle(busHandle) == false )
    {
      transformSet(cAudioTransform.offset);
      continue;
    }

    float amplitude = 0.5f *
      (cSourceBus.bus->getApproximateVolume(0) +
      cSourceBus.bus->getApproximateVolume(1));

    if ( cAudioTransform.ignoreBusVolume == false )
      amplitude *= soloud.getVolume(busHandle);

    cAudioTransform.amplitude += (amplitude - cAudioTransform.amplitude) *
                                 (1.0f - cAudioTransform.damp);

    const auto axis = cAudioTransform.axis;

    const auto factor = std::fma(cAudioTransform.amplitude,
                                 cAudioTransform.magnitude,
                                 cAudioTransform.offset);

    transformSet(factor);
  }
}

} // namespace cqde::systems
