#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace demo
{

void
audioDemoReset(
  entt::registry&,
  const std::vector <std::any>& args );

void
audioDemoConcertInit(
  entt::registry&,
  const std::vector <std::any>& args );

void
audioDemoConcertShutdown(
  entt::registry&,
  const std::vector <std::any>& args );

void
audioDemoDopplerInit(
    entt::registry&,
    const std::vector <std::any>& args );

void
audioDemoDopplerShutdown(
    entt::registry&,
    const std::vector <std::any>& args );

void
audioListenerFilterInit(
    entt::registry&,
    const std::vector <std::any>& args );

void
engineAudioListenerFilterInit(
    entt::registry&,
    const std::vector <std::any>& args );

void
musicConcertControllerInit(
  entt::registry&,
  const std::vector <std::any>& args );

void
musicConcertControllerReset(
  entt::registry&,
  const std::vector <std::any>& args );

void
musicInstrumentReset(
  entt::registry&,
  const std::vector <std::any>& args );

void
musicInstrumentToggle(
  entt::registry&,
  const std::vector <std::any>& args );

void
playDialogue(
  entt::registry&,
  const std::vector <std::any>& args );

void
toggleDialoguePause(
  entt::registry&,
  const std::vector <std::any>& args );

void
playFootstepSound(
  entt::registry&,
  const std::vector <std::any>& args );


void
carReset(
  entt::registry&,
  const std::vector <std::any>& args );


void
audioDemoEngineInit(
  entt::registry&,
  const std::vector <std::any>& args );

void
audioDemoEngineShutdown(
  entt::registry&,
  const std::vector <std::any>& args );

void
engineThrottle(
  entt::registry&,
  const std::vector <std::any>& args );

void
engineCylinderHit(
  entt::registry&,
  const std::vector <std::any>& args );

} // namespace demo
