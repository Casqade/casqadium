#include <SDL_events.h>
#include <SDL_video.h>

#include <Sdl2Wrapper/Animation.hpp>
#include <TimeUtils/Duration.hpp>

#include <cassert>


Animation::Animation( const SDL_Texture*  texture,
                      const SDL_RectVec&  srcRects,
                      const double        fps,
                      const bool          repeat )
  : mFramesPerSec(fps)
  , mFinalFrame(0)
  , mFinished(false)
  , mTimer(1.0 / fps)
  , mRepeat(repeat)
  , Texture(texture, srcRects)
{}

Animation::Animation( const SDL_Texture*  texture,
                      const SDL_Point     frameSize,
                      const double        fps,
                      const bool          repeat )
  : mFramesPerSec(fps)
  , mFinalFrame(0)
  , mFinished(false)
  , mTimer(1.0 / fps)
  , mRepeat(repeat)
  , Texture(texture)
{
  SDL_RectVec rects;
  const SDL_Point textureSize = Sdl2::TextureSize( texture );

  assert( frameSize.x <= textureSize.x && frameSize.y <= textureSize.y );
  assert( int64_t( textureSize.x / frameSize.x ) * ( textureSize.y / frameSize.y ) <= rects.max_size());

  for ( int32_t frameY =  0;
                frameY <  textureSize.y;
                frameY += frameSize.y )
    for ( int32_t frameX =  0;
                  frameX <  textureSize.x;
                  frameX += frameSize.x )
    {
      rects.push_back( {  frameX, frameY,
                          frameSize.x, frameSize.y } );
    }
  mSrcRect = rects;
}

void
Animation::update( const Duration& deltaTime )
{
  if ( mFinished )
    return;

  if ( mTimer.isReady() == false )
    return mTimer.update( deltaTime );

  if ( mFrame == mSrcRect.size() - 1 )
  {
    if ( mRepeat )
      mFrame = 0;
    else
    {
      mFrame = mFinalFrame;
      mFinished = true;
    }
  }
  else
    mFrame++;

  return mTimer.restart();
}

bool
Animation::frameSet( const uint64_t frame )
{
  if ( frame < mSrcRect.size() )
    mFrame = frame;
  else
    return false;

  return true;
}

uint64_t
Animation::frame() const
{
  return mFrame;
}
