#include <SDL.h>

#include <Sdl2Wrapper/Window.hpp>
//#include <Logger/Logger.hpp>


namespace SDL2
{

namespace
{
static const std::string WINDOW_SHOW_ERROR_FAILED = "Failed to show error window: {}";
static const std::string WINDOW_SHOW_WARNING_FAILED = "Failed to show warning window: {}";
static const std::string WINDOW_CREATE_FAILED = "Failed to create window: {}";
static const std::string WINDOW_CREATE_PROGRESS = "Creating window ({}x{}) at ({},{})";
static const std::string WINDOW_CREATE_SUCCESS = "Created window {} ({}x{}) at ({},{})";
static const std::string WINDOW_GET_ID_FAILED = "Failed to get ID of NULL window";
static const std::string WINDOW_DESTROY_PROGRESS = "Destroying window {}";
static const std::string WINDOW_DESTROY_WHEN_RENDERER_PRESENT = "Destroying window {} with active renderer";
}

bool
Window::ShowError(  const std::string&  title,
                    const std::string&  message,
                    SDL_Window*         window )
{
  if ( SDL_ShowSimpleMessageBox(  SDL_MESSAGEBOX_ERROR,
                                  title.c_str(),
                                  message.c_str(),
                                  window ) != 0 )
  {
//    LOG_ERROR( WINDOW_SHOW_ERROR_FAILED, SDL_GetError() );
    return false;
  }

  return true;
}

bool
Window::ShowWarning(  const std::string&  title,
                      const std::string&  message,
                      SDL_Window*         window )
{
  if ( SDL_ShowSimpleMessageBox(  SDL_MESSAGEBOX_WARNING,
                                  title.c_str(),
                                  message.c_str(),
                                  window ) != 0 )
  {
//    LOG_ERROR( WINDOW_SHOW_WARNING_FAILED, SDL_GetError() );
    return false;
  }

  return true;
}

Window::~Window()
{
  mWindow.reset();
}

bool
Window::open( const std::string& title,
              const SDL_Point size,
              const SDL_Point pos,
              const std::forward_list <uint32_t> flags )
{
  mWindow.reset();

  uint32_t windowFlags = 0;
  for ( const auto flag : flags )
    windowFlags |= flag;

  mWindow = std::shared_ptr <SDL_Window>
  (
    SDL_CreateWindow( title.c_str(),
                      pos.x, pos.y,
                      size.x, size.y,
                      windowFlags ),
    [] ( SDL_Window* window )
    {
      SDL_DestroyWindow( window );
    }
  );

  if ( mWindow == nullptr )
  {
    ShowError(  WINDOW_CREATE_FAILED.c_str(),
                SDL_GetError() );
  }


  return true;
}

bool
Window::close()
{
  mWindow.reset();
  return true;
}

void
Window::move( const SDL_Point pos,
              int32_t displayIndex )
{
  if ( mWindow == nullptr )
    return;

  if ( displayIndex < 0 )
    displayIndex = SDL_GetWindowDisplayIndex( mWindow.get() );

  SDL_Rect bounds {};
  if ( SDL_GetDisplayBounds( displayIndex, &bounds ) == 0 )
    return;

  if ( mWindow )
    SDL_SetWindowPosition( mWindow.get(), bounds.x + pos.x, bounds.y + pos.y );
}

void
Window::toCenter()
{
  if ( mWindow == nullptr )
    return;

  int32_t width = 0, height = 0;
  SDL_GetWindowSize( mWindow.get(), &width, &height );

//    move( { SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex), SDL_WINDOWPOS_CENTERED_DISPLAY(displayIndex) }, displayIndex );
}

void
Window::setMinSize( const SDL_Point size )
{
  if ( mWindow )
    SDL_SetWindowMinimumSize( mWindow.get(), size.x, size.y );
}

void
Window::setMaxSize( const SDL_Point size )
{
  if ( mWindow )
    SDL_SetWindowMaximumSize( mWindow.get(), size.x, size.y );
}

SDL_Point
Window::size() const
{
  SDL_Point windowSize;
  SDL_GetWindowSize( mWindow.get(), &windowSize.x, &windowSize.y );

  return windowSize;
}

SDL_Point
Window::pos() const
{
  SDL_Point windowPos;
  SDL_GetWindowPosition( mWindow.get(), &windowPos.x, &windowPos.y );

  return windowPos;
}

Window::operator SDL_Window* ()
{
  return mWindow.get();
}

Window::operator std::weak_ptr <SDL_Window> ()
{
  return mWindow;
}

uint32_t
Window::id() const
{
  if ( mWindow != nullptr )
    return SDL_GetWindowID( mWindow.get() );

//  LOG_ERROR( WINDOW_GET_ID_FAILED, SDL_GetError() );
  return 0;
}

} // namespace SDL2
