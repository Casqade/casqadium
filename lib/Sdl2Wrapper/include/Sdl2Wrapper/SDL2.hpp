#ifndef SDL2_WRAPPER_HPP
#define SDL2_WRAPPER_HPP

#include <TimeUtils/Duration.hpp>

#include <SDL.h>

#ifdef SDL_PNG_SUPPORT
  #include <SDL_image.h>
#endif

#ifdef SDL_MIX_SUPPORT
  #include <SDL_mixer.h>
#endif

#ifdef SDL_TTF_SUPPORT
  #include <SDL_ttf.h>
#endif

#include <string>
#include <map>
#include <memory>
#include <forward_list>


namespace SDL2
{

struct MouseState
{
  int x;
  int y;

  bool left;
  bool middle;
  bool right;
  bool x1;
  bool x2;

  MouseState();
  bool operator == ( const MouseState& ) const;
  operator SDL_Point() const;
};

class Wrapper
{
  using Duration = TimeUtils::Duration;

  bool mInitialized;
  bool mSoundInitialized;

  bool mAllowWindowResize;
  bool mWindowResized;

  int32_t   mUserDisplayIndex;
  SDL_Point mUserScreenSize;
  SDL_Point mUserWindowSize;

  SDL_Window*     mWindow;
  SDL_Renderer*   mRenderer;
  SDL_DisplayMode mUserDisplayMode;
  SDL_Event       mSdlEvent;

  std::map <std::string, SDL_Texture*>  mTextures;

#ifdef SDL_TTF_SUPPORT
  std::map <std::string, TTF_Font*>     mFonts;
#endif

#ifdef SDL_MIX_SUPPORT
  std::map <std::string, Mix_Chunk*>    mSounds;
  std::map <std::string, Mix_Music*>    mMusic;
#endif

  SDL_Point getScreenResolution( const int32_t displayIndex = 0 );
  void updateScreenResolution( const int32_t displayIndex = 0 );

  void logLibVer();

public:
  typedef std::map <const std::string,
                    SDL_Texture*>::const_iterator IterTexture;

#ifdef SDL_TTF_SUPPORT
  typedef std::map <const std::string,
                    TTF_Font*>::const_iterator IterFont;
#endif

#ifdef SDL_MIX_SUPPORT
  typedef std::map <const std::string,
                    Mix_Chunk*>::const_iterator IterSound;

  typedef std::map <const std::string,
                    Mix_Music*>::const_iterator IterMusic;
#endif

  Wrapper();
  ~Wrapper();
//  SDL2( const SDL2& )               = delete;
//  SDL2& operator = ( const SDL2& )  = delete;

  bool init( SDL_Point          windowSize = { 0, 0 },
             const std::string& windowTitle = "Sdl2Wrapper" );

  void close();

  bool grabMouseInputOn();
  bool grabMouseInputOff();

  bool showError( const std::string& title,
                  const std::string& message ) const;

  bool showWarning( const std::string& title,
                    const std::string& message ) const;


  IterTexture loadTexture(  const std::string& name,
                            const std::string& path );
  void unloadTexture( const std::string& name );

#ifdef SDL_TTF_SUPPORT
  IterFont  loadFont( const std::string&  name,
                      const std::string&  path,
                      const uint32_t      size,
                      const int32_t       index = 0 );
  void unloadFont( const std::string& name );
#endif

#ifdef SDL_MIX_SUPPORT
  IterSound loadSound(  const std::string& name,
                        const std::string& path );
  IterMusic loadMusic(  const std::string& name,
                        const std::string& path );

  void unloadSound( const std::string& name );
  void unloadMusic( const std::string& name );
#endif

  void renderTexture( const SDL_Texture*  texture,
                      const SDL_Rect&     dstRect,
                      const SDL_Rect*     srcRect = NULL,
                      const double        angle = 0.0,
                      const SDL_Point*    center = NULL,
                      const SDL_RendererFlip = SDL_FLIP_NONE ) const;

#ifdef SDL_TTF_SUPPORT
  void drawText(  const std::string&  text,
                  const SDL_Point     rect,
                  const TTF_Font*     font,
                  const float         size,
                  const SDL_Color = SDL_Color { 255,
                                                255,
                                                255,
                                                255 } ) const;
#endif

#ifdef SDL_MIX_SUPPORT
  int32_t soundPlay(  const Mix_Chunk*  sound,
                      const int32_t     channel = -1,
                      const int32_t     loopCount = 0 ) const;

  int32_t soundPlayFadeIn(  const Mix_Chunk*  sound,
                            const uint32_t    fadeInMs,
                            const int32_t     channel = -1,
                            const int32_t     loopCount = 0 ) const;

  int32_t soundStop( const int32_t channel ) const;
  int32_t soundStopFadeOut( const int32_t   channel,
                            const uint32_t  fadeOutMs ) const;

  int32_t soundStopAll() const;
  int32_t soundStopAllFadeOut( const uint32_t fadeOutMs ) const;

  void soundPause( const int32_t channel ) const;
  void soundPauseAll() const;

  void soundResume( const int32_t channel ) const;
  void soundResumeAll() const;
#endif

  bool updateEvents();
  void renderUpdate() const;

  static SDL_Point TextureSize( const SDL_Texture* );

  static bool PointInRect(  SDL_Point,
                            SDL_Rect );


  bool setWindowTitle( const std::string& ) const;
  std::string windowTitle() const;

  bool windowResized();
  SDL_Point updateWindowSize();
  SDL_Point windowSize() const;

  int32_t windowHeight()  const;
  int32_t windowWidth()   const;

  int32_t screenHeight()  const;
  int32_t screenWidth()   const;

  MouseState mouseState() const;

  SDL_Event     event()     const;
  SDL_Window*   window()    const;
  SDL_Renderer* renderer()  const;

  bool isInitialized() const;
  bool isSoundInitialized() const;

  IterTexture texture(  const std::string& ) const;

#ifdef SDL_TTF_SUPPORT
  IterFont font(     const std::string& ) const;
#endif

#ifdef SDL_MIX_SUPPORT
  IterSound sound(    const std::string& ) const;
  IterMusic music(    const std::string& ) const;
#endif
};

} // namespace SDL2
using Sdl2 = SDL2::Wrapper;

#endif
