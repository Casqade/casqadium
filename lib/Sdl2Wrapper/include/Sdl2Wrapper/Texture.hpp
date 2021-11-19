#ifndef SDL2_TEXTURE_HPP
#define SDL2_TEXTURE_HPP

#include <vector>

#include <Sdl2Wrapper/SDL2.hpp>


class Texture
{
private:
//  Texture();

protected:
  typedef std::vector <SDL_Rect> SDL_RectVec;

  SDL_Texture*  mTexture;
  SDL_RectVec   mSrcRect;
  SDL_Point     mCenter;
  uint8_t       mFrame;

public:
  ~Texture();
  Texture(  const SDL_Texture* );
  Texture(  const SDL_Texture*,
            const SDL_RectVec& );

  void render(  const SDL_Rect*,
                const double angle = 0.0 ) const;

  SDL_Texture*  texture() const;
  SDL_Rect*     rect()    const;
};


#endif
