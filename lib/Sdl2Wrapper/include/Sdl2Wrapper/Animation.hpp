#ifndef SDL2_ANIMATION_HPP
#define SDL2_ANIMATION_HPP

#include <Sdl2Wrapper/Texture.hpp>
#include <TimeUtils/Timer.hpp>


class Animation : public Texture
{
  double   mFramesPerSec;
  uint64_t mFinalFrame;
  bool    mFinished;
  bool    mRepeat;
  Timer   mTimer;

  using Duration = TimeUtils::Duration;

public:
  Animation(  const SDL_Texture*,
              const double,
              const bool = false);

  Animation(  const SDL_Texture*,
              const SDL_RectVec&,
              const double,
              const bool = false);

  Animation(  const SDL_Texture*,
              const SDL_Point,
              const double,
              const bool = false );

  void update( const Duration& );
  bool frameSet( const uint64_t );

  uint64_t frame() const;
};


#endif
