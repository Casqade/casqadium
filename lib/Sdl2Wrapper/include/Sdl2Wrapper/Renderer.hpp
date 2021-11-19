#ifndef SDL2_RENDERER_HPP
#define SDL2_RENDERER_HPP

#include <memory>
#include <cstdint>
#include <forward_list>
#include <map>

#include <SDL_render.h>


namespace SDL2
{

class Renderer
{
  std::shared_ptr <SDL_Renderer> mRenderer;

  std::map <std::string, SDL_Texture*> mTextures;
  typedef std::map <const std::string,
                    SDL_Texture*>::const_iterator IterTexture;

public:
  ~Renderer();

  bool create( const std::weak_ptr <SDL_Window> targetWindow,
               const int32_t renderDriverIndex = -1,
               const std::forward_list <uint32_t> flags = {} );

  void close();


  IterTexture loadTexture(  const std::string& name,
                            const std::string& path );
  IterTexture texture( const std::string& ) const;

  SDL_Point size() const;

  operator SDL_Renderer* () const;
  operator std::weak_ptr <SDL_Renderer> () const;
};

} // namespace SDL2


#endif
