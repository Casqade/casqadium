#ifndef SDL2_SOUND_HPP
#define SDL2_SOUND_HPP

#include <Sdl2Wrapper/SDL2.hpp>


class Sound
{
  Mix_Chunk*    mSound;
  uint8_t       mVolume;
  int32_t       mChannel;
  int32_t       mLoops;
  uint32_t      mFadeInMs;
  uint32_t      mFadeOutMs;
  bool          mRepeating;

public:
  ~Sound();
  Sound(  const Mix_Chunk*,
          const uint8_t   = MIX_MAX_VOLUME,
          const int32_t   = -1,
          const int32_t   = 0,
          const uint32_t  = 0,
          const uint32_t  = 0,
          const bool      = false );

  int32_t play();
  int32_t playFadeIn();

  bool    stop()        const;
  bool    stopFadeOut() const;

  void    pause()   const;
  void    resume()  const;

  uint8_t setVolume( const uint8_t );
  bool    setRepeating( const bool );
};


#endif
