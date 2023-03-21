#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/logger.hpp>

#include <cqde/types/audio/AudioFilterFactory.hpp>

#include <soloud_bassboostfilter.h>
#include <soloud_biquadresonantfilter.h>
#include <soloud_duckfilter.h>
#include <soloud_echofilter.h>
#include <soloud_eqfilter.h>
#include <soloud_flangerfilter.h>
#include <soloud_freeverbfilter.h>
#include <soloud_lofifilter.h>
#include <soloud_robotizefilter.h>
#include <soloud_waveshaperfilter.h>


namespace cqde::types
{

bool
CasqadiumEngine::initAudio()
{
  LOG_DEBUG("Initializing SoLoud");

  auto& audioBackend = mRegistry.ctx().emplace <SoLoud::Soloud> ();

  const auto audioInitResult = audioBackend.init(
    mConfig->audioFlags(),
    mConfig->audio.backend,
    mConfig->audio.sampleRate,
    mConfig->audio.bufferSize );

  if ( audioInitResult != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR )
  {
    LOG_ERROR("Failed to initialize SoLoud: {}",
              audioBackend.getErrorString(audioInitResult));

    return false;
  }

  audioBackend.setMainResampler(mConfig->audio.resampler);
  audioBackend.setMaxActiveVoiceCount(mConfig->audio.maxActiveVoices);

  auto& audioFilterFactory = mRegistry.ctx().emplace <AudioFilterFactory> ();

  audioFilterFactory.registerFilter <SoLoud::BassboostFilter> ("BassboostFilter");
  audioFilterFactory.registerFilter <SoLoud::BiquadResonantFilter> ("BiquadResonantFilter");
  audioFilterFactory.registerFilter <SoLoud::DuckFilter> ("DuckFilter");
  audioFilterFactory.registerFilter <SoLoud::EchoFilter> ("EchoFilter");
  audioFilterFactory.registerFilter <SoLoud::EqFilter> ("EqFilter");
  audioFilterFactory.registerFilter <SoLoud::FlangerFilter> ("FlangerFilter");
  audioFilterFactory.registerFilter <SoLoud::FreeverbFilter> ("FreeverbFilter");
  audioFilterFactory.registerFilter <SoLoud::LofiFilter> ("LofiFilter");
  audioFilterFactory.registerFilter <SoLoud::RobotizeFilter> ("RobotizeFilter");
  audioFilterFactory.registerFilter <SoLoud::WaveShaperFilter> ("WaveShaperFilter");

  return true;
}

} // namespace cqde::types
