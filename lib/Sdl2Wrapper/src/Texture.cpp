#include <SDL.h>

#include <Sdl2Wrapper/Texture.hpp>


Texture::~Texture()
{
  SDL_DestroyTexture( mTexture );
}

Texture::Texture( const SDL_Texture* texture )
  : Texture( texture, {} )
{}

Texture::Texture( const SDL_Texture* texture,
                  const SDL_RectVec& srcRect )
  : mTexture((SDL_Texture*) texture)
  , mSrcRect(srcRect)
  , mCenter()
  , mFrame(0)
{}

void
Texture::render(  const SDL_Rect* dstRect,
                  const double    angle ) const
{
//  sdl2.renderTexture( mTexture,
//                      dstRect,
//                      &mSrcRect[mFrame],
//                      angle );
}

SDL_Texture*
Texture::texture() const
{
  return mTexture;
}

SDL_Rect*
Texture::rect() const
{
  return (SDL_Rect*) &mSrcRect[mFrame];
}
