#include <demo/callbacks.hpp>
#include <demo/components.hpp>

#include <cqde/audio_helpers.hpp>
#include <cqde/math_helpers.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/callbacks/audio.hpp>
#include <cqde/callbacks/common_routine.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/audio/AudioFilterFactory.hpp>

#include <cqde/components/audio/AudioBus.hpp>
#include <cqde/components/audio/AudioLoop.hpp>
#include <cqde/components/audio/Audio3dParams.hpp>
#include <cqde/components/audio/AudioGroupMap.hpp>
#include <cqde/components/audio/AudioListener3d.hpp>
#include <cqde/components/assets/AudioAssetList.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/EntityList.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/InteractionListenerColor.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/collision/CollisionCallback.h>

#include <cqde/logger.hpp>
#include <bitset>


namespace demo
{

static void
ClearAllSounds(
  entt::registry& registry )
{
  using cqde::AudioHandleInvalid;
  using cqde::compos::AudioBus;
  using cqde::compos::AudioListener3d;
  using cqde::compos::SubscriberUpdate;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( auto&& [eListener, cListener, cListenerBus]
          : registry.view <AudioListener3d, AudioBus> ().each() )
  {
    for ( const auto& [handle, entity] : cListener.instances )
      soloud.stop(handle);

    cListenerBus.bus->stop();
    cListener.instances.clear();
  }
}

void
audioDemoReset(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::Transform;
  using cqde::compos::InputController;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::CallbackManager;
  using cqde::callbacks::audioStopAll;
  using cqde::callbacks::entityUpdateOn;
  using cqde::callbacks::entitiesUpdateOn;

  audioStopAll(registry, {});
  ClearAllSounds(registry);

  audioDemoConcertShutdown(registry, {});
  audioDemoDopplerShutdown(registry, {});
  audioDemoEngineShutdown(registry, {});

  for ( auto&& [eController, cInputController, cTransform]
          : registry.view <InputController, Transform> ().each() )
    cTransform = {};

  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  callbackManager.executeLater(
  [] (  entt::registry& registry,
        const CallbackManager::CallbackArgs& args )
  {
    registry.clear <SubscriberUpdate> ();
    entitiesUpdateOn(registry, args);
    entityUpdateOn(registry, {args});

  }, {args} );
}

void
audioDemoConcertInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::SubscriberUpdate;
  using cqde::types::CallbackManager;
  using cqde::callbacks::entitiesUpdateOn;

  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  callbackManager.executeLater(
  [] (  entt::registry& registry,
        const CallbackManager::CallbackArgs& args )
  {
    registry.clear <SubscriberUpdate> ();
    entitiesUpdateOn(registry, args);
    audioListenerFilterInit(registry, args);

  }, {args} );
}

void
audioDemoConcertShutdown(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::InteractionListenerColor;

  for ( auto&& [eInstrument, cInstrument, cInstrumentColor]
          : registry.view <MusicalInstrument, InteractionListenerColor> ().each() )
    musicInstrumentReset(registry, {eInstrument});

  for ( auto&& [eConcertController, cConcertController]
          : registry.view <MusicConcertController> ().each() )
    musicConcertControllerReset(registry, {eConcertController});
}

void
audioDemoDopplerInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::AudioLoop;
  using cqde::compos::AudioListener3d;
  using cqde::compos::Transform;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::CallbackManager;
  using cqde::types::AudioAssetManager;
  using cqde::callbacks::entitiesUpdateOn;
  using cqde::updateAudio3dParams;
  using cqde::calcAudioListenerParams;

  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  callbackManager.executeLater(
  [] (  entt::registry& registry,
        const CallbackManager::CallbackArgs& args )
  {
    registry.clear <SubscriberUpdate> ();
    entitiesUpdateOn(registry, args);
    audioListenerFilterInit(registry, args);

    carReset(registry, args);

    auto& soloud = registry.ctx().get <SoLoud::Soloud> ();
    const auto& audioManager = registry.ctx().get <AudioAssetManager> ();

    for ( const auto&& [eCar, cCarLoop, cCarTransform]
            : registry.view <AudioLoop, const Transform, SubscriberUpdate> ().each() )
    {
      if ( soloud.isVoiceGroup(cCarLoop.handle) == false )
        cCarLoop.handle = soloud.createVoiceGroup();

      for ( auto& layer : cCarLoop.layers )
      {
        if ( soloud.isValidVoiceHandle(layer.handle) == true )
          continue;

        const auto audio = audioManager.try_get(layer.source);

        if ( audio == nullptr )
          continue;

        for ( const auto&& [eListener, cListener, cListenerBus, cListenerTransform]
                : registry.view <AudioListener3d, AudioBus, const Transform, SubscriberUpdate> ().each() )
        {
          cListenerBus.ensureIsPlaying(soloud);

          const auto handle = cListenerBus.bus->play3d( *audio,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, true );

          soloud.addVoiceToGroup(cCarLoop.handle, handle);

          const auto listenerParams = calcAudioListenerParams(
            registry,
            eListener,
            cListenerTransform );

          updateAudio3dParams(
            registry,
            handle,
            listenerParams,
            eListener,
            cListenerTransform,
            eCar,
            cCarTransform);

          layer.handle = handle;
          cListener.instances[cCarLoop.handle] = eCar;
        }

        soloud.setLooping(cCarLoop.handle, true);
        soloud.setInaudibleBehavior(cCarLoop.handle, false, false);

        soloud.update3dAudio();
        soloud.setPause(cCarLoop.handle, false);
      }
    }

  }, {args} );
}

void
audioDemoDopplerShutdown(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioLoop;
  using cqde::compos::Transform;
  using cqde::compos::Transform;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( const auto&& [eCar, cCarLoop, cCarTransform]
          : registry.view <AudioLoop, const Transform> ().each() )
    soloud.destroyVoiceGroup(cCarLoop.handle);
}

void
audioDemoEngineInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::SubscriberUpdate;
  using cqde::types::CallbackManager;
  using cqde::callbacks::entitiesUpdateOn;

  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  callbackManager.executeLater(
  [] (  entt::registry& registry,
        const CallbackManager::CallbackArgs& args )
  {
    registry.clear <SubscriberUpdate> ();
    entitiesUpdateOn(registry, args);
    engineAudioListenerFilterInit(registry, args);

  }, {args} );
}

void
audioDemoEngineShutdown(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
}

void
audioListenerFilterInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::AudioListener3d;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::AudioFilterFactory;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  const auto& filterFactory = registry.ctx().get <AudioFilterFactory> ();

  const auto filter = filterFactory.get("FreeverbFilter");

  if ( filter == nullptr )
    return;

  for ( const auto&& [eListener, cListener, cListenerBus]
          : registry.view <AudioListener3d, AudioBus, SubscriberUpdate> ().each() )
  {
    cListenerBus.ensureIsPlaying(soloud);
    const auto busHandle = cListenerBus.bus->mChannelHandle;

    cListenerBus.bus->setFilter(0, filter);

    soloud.setFilterParameter(
      busHandle,
      0, 0, 0.25f );

    soloud.setFilterParameter(
      busHandle,
      0, 2, 0.5f );

    soloud.setFilterParameter(
      busHandle,
      0, 3, 0.25f );

    soloud.setFilterParameter(
      busHandle,
      0, 4, 0.5f );
  }
}

void
engineAudioListenerFilterInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::AudioListener3d;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::AudioFilterFactory;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  const auto& filterFactory = registry.ctx().get <AudioFilterFactory> ();

  const auto filter = filterFactory.get("BiquadResonantFilter");

  if ( filter == nullptr )
    return;

  for ( const auto&& [eListener, cListener, cListenerBus]
          : registry.view <AudioListener3d, AudioBus, SubscriberUpdate> ().each() )
  {
    cListenerBus.ensureIsPlaying(soloud);
    const auto busHandle = cListenerBus.bus->mChannelHandle;

    cListenerBus.bus->setVisualizationEnable(true);
    cListenerBus.bus->setFilter(0, filter);

    soloud.setFilterParameter(
      busHandle,
      0, 0, 1.0f );

    soloud.setFilterParameter(
      busHandle,
      0, 2, 1000.0f );

    soloud.setFilterParameter(
      busHandle,
      0, 3, 0.95f );
  }
}

void
musicConcertControllerInit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::AudioListener3d;
  using cqde::compos::Tag;
  using cqde::compos::EntityList;
  using cqde::compos::Transform;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::AudioAssetManager;
  using cqde::AudioHandleInvalid;

  const auto eConcertController = std::any_cast <entt::entity> (args.at(0));

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();
  const auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  auto& cConcertController = registry.get <MusicConcertController> (eConcertController);
  auto& cInstrumentList = registry.get <const EntityList> (eConcertController);

  for ( const auto&& [eListener, cListener, cListenerBus, cListenerTransform]
          : registry.view <AudioListener3d, AudioBus, Transform, SubscriberUpdate> ().each() )
  {
    cListenerBus.ensureIsPlaying(soloud);

    if ( soloud.isVoiceGroup(cConcertController.groupHandle) == false )
      cConcertController.groupHandle = soloud.createVoiceGroup();

    for ( const auto& instrument : cInstrumentList.entities )
    {
      const auto eInstrument = instrument.get(registry);

      if ( eInstrument == entt::null )
        continue;

      auto&& [cInstrument, cInstrumentBus]
        = registry.get <MusicalInstrument, AudioBus> (eInstrument);

      if ( soloud.isValidVoiceHandle(cInstrument.handle) == true )
        continue;

      const auto audio = audioManager.try_get(cInstrument.audioId);

      if ( audio == nullptr )
        continue;

      if ( soloud.isValidVoiceHandle(cInstrumentBus.bus->mChannelHandle) == false )
        cInstrument.handle = cListenerBus.bus->play3d(
          *cInstrumentBus.bus,
          0.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 0.0f,
          0.0f, true );

      cInstrumentBus.bus->setVisualizationEnable(true);

      const auto audioHandle = cInstrumentBus.bus->play(*audio);
      soloud.setLooping(audioHandle, true);

      cListener.instances[cInstrument.handle] = eInstrument;
      soloud.addVoiceToGroup(cConcertController.groupHandle,
                             cInstrument.handle);
      soloud.setInaudibleBehavior(cInstrument.handle, true, false);
    }
  }
}

void
musicConcertControllerReset(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::AudioHandleInvalid;
  using cqde::compos::InteractionListenerColor;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( auto&& [eInstrument, cInstrument, cInstrumentColor]
          : registry.view <MusicalInstrument, InteractionListenerColor> ().each() )
    musicInstrumentReset(registry, {eInstrument});

  const auto eConcertController = std::any_cast <entt::entity> (args.at(0));

  auto& cConcertController
    = registry.get <MusicConcertController> (eConcertController);

  soloud.stop(cConcertController.groupHandle);
  soloud.destroyVoiceGroup(cConcertController.groupHandle);
  cConcertController.groupHandle = AudioHandleInvalid;
}

void
musicInstrumentReset(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::AudioHandleInvalid;
  using cqde::compos::AudioBus;
  using cqde::compos::InteractionListenerColor;

  const auto eInstrument = std::any_cast <entt::entity> (args.at(0));

  auto&& [cInstrument, cInstrumentBus, cInstrumentColor]
    = registry.get <MusicalInstrument, AudioBus, InteractionListenerColor> (eInstrument);

  cInstrument.handle = AudioHandleInvalid;
  cInstrumentColor.color = olc::RED;
}

void
musicInstrumentToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::SubscriberUpdate;
  using cqde::compos::InteractionListenerColor;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( auto&& [eController, cController]
          : registry.view <MusicConcertController, SubscriberUpdate> ().each() )
  {
    musicConcertControllerInit(registry, {eController});
    soloud.setPause(cController.groupHandle, false);
    break;
  }

  const auto eInstrument = std::any_cast <entt::entity> (args.at(0));

  auto&& [cInstrument, cInstrumentColor]
    = registry.get <MusicalInstrument,
                    InteractionListenerColor> (eInstrument);

  const auto instrumentVolume = soloud.getVolume(cInstrument.handle);

  if ( instrumentVolume == 0.0f )
  {
    cInstrumentColor.color = olc::GREEN;
    soloud.fadeVolume(cInstrument.handle, 1.0f, 2.0f);
  }
  else if ( instrumentVolume == 1.0f )
  {
    cInstrumentColor.color = olc::RED;
    soloud.fadeVolume(cInstrument.handle, 0.0f, 2.0f);
  }
}

void
playDialogue(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::updateAudio3dParams;
  using cqde::calcAudioListenerParams;
  using cqde::compos::Transform;
  using cqde::compos::SubscriberUpdate;
  using cqde::compos::AudioBus;
  using cqde::compos::AudioAssetList;
  using cqde::compos::AudioListener3d;
  using cqde::types::AudioAssetManager;

  const auto eDrunkard = std::any_cast <entt::entity> (args.at(0));

  auto& cDrunkardTransform = registry.get <const Transform> (eDrunkard);

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  auto& cDrunkardBus = registry.get <AudioBus> (eDrunkard);

  if ( soloud.isValidVoiceHandle(cDrunkardBus.bus->mChannelHandle) == false )
    for ( const auto&& [eListener, cListener, cListenerBus, cListenerTransform]
            : registry.view <AudioListener3d, AudioBus, const Transform, SubscriberUpdate> ().each() )
    {
      for ( const auto& [handle, entity] : cListener.instances )
        if ( eDrunkard == entity )
          return;

      cListenerBus.ensureIsPlaying(soloud);

      const auto handle = cListenerBus.bus->play3d( *cDrunkardBus.bus,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, true );

      const auto listenerParams = calcAudioListenerParams(
        registry,
        eListener,
        cListenerTransform );

      updateAudio3dParams(
        registry,
        handle,
        listenerParams,
        eListener,
        cListenerTransform,
        eDrunkard,
        cDrunkardTransform);

      soloud.update3dAudio();
      soloud.setPause(handle, false);

      cListener.instances[handle] = eDrunkard;
    }

  if ( cDrunkardBus.bus->getActiveVoiceCount() > 0 )
    return;

  auto& dialogueList = registry.get <const AudioAssetList> (eDrunkard);

  if ( dialogueList.audio.empty() == true )
    return;

  const auto index = cqde::random(size_t{}, dialogueList.audio.size() - 1);
  auto iter = dialogueList.audio.begin();
  std::advance(iter, index);

  if ( iter == dialogueList.audio.end() )
    return;

  const auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  const auto audio = audioManager.try_get(*iter);

  if ( audio == nullptr )
    return;

  cDrunkardBus.bus->setVisualizationEnable(true);
  const auto handle = cDrunkardBus.bus->play(*audio);

  soloud.setAutoStop(handle, true);
  soloud.setInaudibleBehavior(handle, true, false);
}

void
toggleDialoguePause(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::EntityList;

  const auto eDrunkard = std::any_cast <entt::entity> (args.at(0));

  auto& cEntityList = registry.get <const EntityList> (eDrunkard);

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( const auto& bus : cEntityList.entities )
  {
    const auto eBus = bus.get(registry);

    if ( eBus == entt::null )
      continue;

    const auto cBus = registry.try_get <AudioBus> (eBus);

    if ( cBus == nullptr )
      continue;

    auto busHandle = cBus->handle();

    if ( soloud.isValidVoiceHandle(busHandle) == false )
      continue;

    cBus->ensureIsPlaying(soloud);
    busHandle = cBus->handle();

    if ( soloud.getPause(busHandle) == true )
      soloud.setPause(busHandle, false);
    else
      soloud.setPause(busHandle, true);
  }
}

void
playFootstepSound(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::updateAudio3dParams;
  using cqde::calcAudioListenerParams;
  using cqde::compos::Transform;
  using cqde::compos::SubscriberUpdate;
  using cqde::compos::AudioBus;
  using cqde::compos::AudioGroupMap;
  using cqde::compos::AudioListener3d;
  using cqde::types::AudioAssetManager;

  const auto eActor = std::any_cast <entt::entity> (args.at(0));
  const auto eSurface = std::any_cast <entt::entity> (args.at(1));

  auto cSurface = registry.try_get <const PhysicsMaterial> (eSurface);

  if ( cSurface == nullptr )
    return;

  auto& cActorTransform = registry.get <const Transform> (eActor);

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();
  const auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  for ( auto&& [eAudioBank, cAudioMap]
          : registry.view <const AudioGroupMap, const FootstepAudioBank> ().each() )
  {
    auto iter = cAudioMap.groups.find(cSurface->materialId);

    if ( iter == cAudioMap.groups.end() )
      continue;

    auto& audioGroup = iter->second;

    if ( audioGroup.empty() == true )
      continue;

    const auto index = cqde::random(size_t{}, audioGroup.size() - 1);

    const auto audio = audioManager.try_get(audioGroup[index]);

    if ( audio == nullptr )
      continue;

    for ( const auto&& [eListener, cListener, cListenerBus, cListenerTransform]
            : registry.view <AudioListener3d, AudioBus, const Transform, SubscriberUpdate> ().each() )
    {
      cListenerBus.ensureIsPlaying(soloud);

      const auto handle = cListenerBus.bus->play3d( *audio,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        1.0f, true );

      soloud.setAutoStop(handle, true);
      soloud.setInaudibleBehavior(handle, false, true);

      const auto listenerParams = calcAudioListenerParams(
        registry,
        eListener,
        cListenerTransform );

      updateAudio3dParams(
        registry,
        handle,
        listenerParams,
        eListener,
        cListenerTransform,
        eActor,
        cActorTransform);

      soloud.update3dAudio();
      soloud.setPause(handle, false);

      cListener.instances[handle] = eActor;
    }

    return;
  }
}


void
carReset(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::random;
  using cqde::glmToRp3d;
  using cqde::GetWorldMatrix;
  using cqde::compos::Transform;
  using cqde::compos::RigidBody;
  using cqde::compos::EntityList;

  const auto eTrigger = std::any_cast <entt::entity> (args.at(0));

  const auto cCarList = registry.try_get <const EntityList> (eTrigger);

  if ( cCarList == nullptr )
    return;

  for ( const auto& car : cCarList->entities )
  {
    const auto eCar = car.get(registry);

    if ( eCar == entt::null )
      continue;

    const auto&& [cTransform, cBody] = registry.try_get <Transform, RigidBody> (eCar);

    if ( cTransform == nullptr ||
         cBody == nullptr )
      continue;

    cTransform->translation = {};

    const rp3d::Vector3 newVelocity
    {
      random(-90.0f, -150.0f),
      0.0f,
      0.0f,
    };

    const auto newTransform = GetWorldMatrix(registry, eCar, *cTransform);

    cBody->body->setTransform(glmToRp3d(newTransform));
    cBody->body->setLinearVelocity(newVelocity);
  }
}

void
engineThrottle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::types::ControlAxis;
  using cqde::types::TickCurrent;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  const auto axis = std::any_cast <ControlAxis*> (args.at(2));

  auto& cEngine = registry.get <EngineController> (entity);

  cEngine.throttle = std::clamp(axis->value, 0.0f, 1.0f);
};

void
engineCylinderHit(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using cqde::compos::AudioBus;
  using cqde::compos::AudioListener3d;
  using cqde::compos::RigidBody;
  using cqde::compos::Transform;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::AudioAssetManager;

  const auto eEngine = std::any_cast <entt::entity> (args.at(0));

  auto&& [cController, cBody] = registry.try_get <EngineController, RigidBody> (eEngine);

  if ( cController == nullptr ||
       cBody == nullptr )
    return;

  const auto& audioManager = registry.ctx().get <AudioAssetManager> ();

  const auto audio = audioManager.try_get(cController->cylinderAudioId);

  if ( audio == nullptr )
    return;

  auto& soloud = registry.ctx().get <SoLoud::Soloud> ();

  for ( const auto&& [eListener, cListener, cListenerBus, cListenerTransform]
          : registry.view <AudioListener3d, AudioBus, const Transform, SubscriberUpdate> ().each() )
  {
    cListenerBus.ensureIsPlaying(soloud);

    const auto handle = cListenerBus.bus->play( *audio,
      1.0f, 0.0f, true );

    soloud.setAutoStop(handle, true);
    soloud.setInaudibleBehavior(handle, false, true);
    soloud.setVolume(handle, cController->engine.soundVolume.current);
    soloud.setRelativePlaySpeed(handle, cController->engine.soundSpeed.current);
    soloud.setFilterParameter(cListenerBus.handle(), 0, 2, cController->engine.cutoffFrequency.current);

    soloud.setPause(handle, false);
  }
}

} // namespace demo
