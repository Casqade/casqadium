#include <Sdl2Wrapper/Sound.hpp>


Sound::~Sound()
{
  Mix_FreeChunk( mSound );
}

Sound::Sound( const Mix_Chunk*  sound,
              const uint8_t     volume,
              const int32_t     channel,
              const int32_t     loops,
              const uint32_t    fadeInMs,
              const uint32_t    fadeOutMs,
              const bool        repeating )
                : mSound((Mix_Chunk*) sound),
                  mVolume(volume),
                  mChannel(channel),
                  mLoops(loops),
                  mFadeInMs(fadeInMs),
                  mFadeOutMs(fadeOutMs),
                  mRepeating(repeating)
{
  mVolume = std::min(volume, (uint8_t) 100)
            * MIX_MAX_VOLUME / 100.0f;
}

int32_t Sound::play()
{
  int32_t loops = mRepeating ? -1 : mLoops;
//  mChannel = sdl2.soundPlay(  mSound,
//                              mChannel,
//                              loops );
  return mChannel;
}

int32_t Sound::playFadeIn()
{
  int32_t loops = mRepeating ? -1 : mLoops;
//  mChannel = sdl2.soundPlayFadeIn(  mSound,
//                                    mFadeInMs,
//                                    mChannel,
//                                    loops );
  return mChannel;
}

bool Sound::stop() const
{
  if ( mChannel == -1 )
    return false;

  return true;
//  return sdl2.soundStop( mChannel );
}

bool Sound::stopFadeOut() const
{
  if ( mChannel == -1 )
    return false;

  return true;
//  return sdl2.soundStopFadeOut( mChannel,
//                                mFadeOutMs );
}

uint8_t Sound::setVolume( const uint8_t volume )
{
  mVolume = std::min(volume, (uint8_t) 100)
            * MIX_MAX_VOLUME / 100.0f;

  return Mix_VolumeChunk( mSound, mVolume );
}

bool Sound::setRepeating( const bool repeat )
{
  return mRepeating = repeat;
}
