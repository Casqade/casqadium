#include <SDL.h>

#include <Sdl2Wrapper/Renderer.hpp>
#include <Sdl2Wrapper/Window.hpp>
//#include <Logger/Logger.hpp>

#ifdef SDL_PNG_SUPPORT
  #include <SDL_image.h>
#endif


namespace SDL2
{

namespace
{
static const std::string RENDERER_CREATE_PROGRESS = "Creating SDL renderer";
static const std::string RENDERER_CREATE_FAILED = "Failed to create SDL renderer: ";
static const std::string RENDERER_CREATE_FAILED_NO_WINDOW = "Failed to create SDL renderer: target window is NULL";
static const std::string RENDERER_CREATE_FAILED_WINDOW_HAS_RENDERER = "Failed to create SDL renderer: target window already contains one";
static const std::string RENDERER_DESTROY_PROGRESS = "Destroying SDL renderer";
static const std::string RENDERER_GET_OUTPUT_SIZE_FAILED = "Failed to get SDL renderer output size: {}";
static const std::string RENDERER_TEXTURE_LOAD_PROGRESS = "Loading texture '{}' from {}";
static const std::string RENDERER_TEXTURE_LOAD_UNINITIALIZED = "Failed to load texture without renderer";
static const std::string RENDERER_TEXTURE_LOAD_ERROR = "Failed to get SDL renderer output size: {}";
static const std::string RENDERER_TEXTURE_CREATE_FAILED = "Failed to create texture from surface: {}";
static const std::string RENDERER_TEXTURE_ACCESS_FAILED = "Accessing texture '{}' which does not exist";
}

Renderer::~Renderer()
{
  mRenderer.reset();
  for ( const auto& texture : mTextures )
  {
//    LOG_DEBUG( "Freeing Texture {}", texture.first );
    SDL_DestroyTexture( texture.second );
  }
}

bool
Renderer::create( const std::weak_ptr <SDL_Window> targetWindow,
                  const int32_t renderDriverIndex,
                  const std::forward_list <uint32_t> flags )
{
  mRenderer.reset();

  uint32_t rendererFlags = 0;
  for ( const auto flag : flags )
    rendererFlags |= flag;

  if ( targetWindow.expired() == true )
  {
    Window::ShowError( RENDERER_CREATE_FAILED_NO_WINDOW,
                       RENDERER_CREATE_FAILED_NO_WINDOW );
    return false;
  }

  if ( SDL_GetRenderer( targetWindow.lock().get() ) != nullptr )
  {
    Window::ShowError( RENDERER_CREATE_FAILED_WINDOW_HAS_RENDERER,
                       RENDERER_CREATE_FAILED_WINDOW_HAS_RENDERER );

    return false;
  }

  mRenderer = std::shared_ptr <SDL_Renderer>
  (
    SDL_CreateRenderer( targetWindow.lock().get(),
                        renderDriverIndex,
                        rendererFlags ),
    [] ( SDL_Renderer* renderer )
    {
      SDL_DestroyRenderer( renderer );
    }
  );


  if ( mRenderer == nullptr )
  {
    Window::ShowError( RENDERER_CREATE_FAILED,
                       SDL_GetError(),
                       targetWindow.lock().get() );

    return false;
  }

  return true;
}

void
Renderer::close()
{
  mRenderer.reset();
}

Renderer::IterTexture
Renderer::loadTexture(  const std::string& textureName,
                        const std::string& texturePath )
{
  if ( mRenderer == nullptr )
  {
    Window::ShowError(  RENDERER_TEXTURE_LOAD_UNINITIALIZED,
                        RENDERER_TEXTURE_LOAD_UNINITIALIZED );

    return {};
  }

#ifdef SDL_PNG_SUPPORT
  SDL_Surface* loadedSurface = IMG_Load( texturePath.c_str() );
#else
  SDL_Surface* loadedSurface = SDL_LoadBMP( texturePath.c_str() );
#endif

  if ( loadedSurface == NULL )
  {
//    LOG_WARN( RENDERER_TEXTURE_LOAD_ERROR, texturePath,
#ifdef SDL_PNG_SUPPORT
//              IMG_GetError()
#else
//              SDL_GetError()
#endif
//              );

    Window::ShowWarning(  RENDERER_TEXTURE_LOAD_ERROR,
                          texturePath.c_str() );

    return {};
  }

  SDL_Texture* newTexture =
    SDL_CreateTextureFromSurface( mRenderer.get(),
                                  loadedSurface );

  if ( newTexture == NULL )
  {
//    LOG_WARN( RENDERER_TEXTURE_CREATE_FAILED, SDL_GetError() );
    Window::ShowWarning(  RENDERER_TEXTURE_CREATE_FAILED,
                          texturePath.c_str() );

    SDL_FreeSurface( loadedSurface );
    return {};
  }

  SDL_FreeSurface( loadedSurface );
  mTextures[textureName] = newTexture;

  return mTextures.find( textureName );
}

Renderer::IterTexture
Renderer::texture( const std::string& textureName ) const
{
  auto texture = mTextures.find( textureName );
  if ( texture != mTextures.end() )
    return texture;

//  LOG_ERROR( RENDERER_TEXTURE_ACCESS_FAILED, textureName );
  Window::ShowWarning(  RENDERER_TEXTURE_ACCESS_FAILED,
                        textureName );

  return {};
}

SDL_Point
Renderer::size() const
{
  SDL_Point outputSize;
  if ( SDL_GetRendererOutputSize( mRenderer.get(), &outputSize.x, &outputSize.y ) != 0 )
  {
//    LOG_ERROR( RENDERER_GET_OUTPUT_SIZE_FAILED, SDL_GetError() );
    return {};
  }

  return outputSize;
}

Renderer::operator SDL_Renderer* () const
{
  return mRenderer.get();
}

Renderer::operator std::weak_ptr <SDL_Renderer> () const
{
  return mRenderer;
}

} // namespace SDL2
