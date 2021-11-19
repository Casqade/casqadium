#ifndef SDL2_WINDOW_HPP
#define SDL2_WINDOW_HPP

#include <string>
#include <memory>
#include <forward_list>

#include <SDL_video.h>


namespace SDL2
{

class Window
{
  std::shared_ptr <SDL_Window> mWindow;

public:
  ~Window();

  static bool ShowError(  const std::string&  title,
                          const std::string&  message,
                          SDL_Window*         window = nullptr );
  static bool ShowWarning(  const std::string&  title,
                            const std::string&  message,
                            SDL_Window*         window = nullptr );

  bool open(  const std::string& title = {},
              const SDL_Point size = {},
              const SDL_Point pos = { SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED },
              const std::forward_list <uint32_t> flags = {} );
  bool close();

  void move( const SDL_Point pos,
             int32_t displayIndex = -1 );
  void toCenter();
  void setMinSize( const SDL_Point size );
  void setMaxSize( const SDL_Point size );

  SDL_Point size() const;
  SDL_Point pos() const;

  operator SDL_Window* ();
  operator std::weak_ptr <SDL_Window> ();

  uint32_t id() const;
};

} // namespace SDL2


#endif
