#include <Logger/Logger.hpp>
#include <Sdl2Wrapper/SDL2.hpp>
#include <TimeUtils/Duration.hpp>


namespace
{
static auto logger = Logger::instance();

static std::shared_ptr <spdlog::logger>
UserLogger()
{
  return Logger::Register( "SDL2", spdlog::level::trace );
}
}


namespace SDL2
{

MouseState::MouseState()
  : x()
  , y()
  , left()
  , middle()
  , right()
  , x1()
  , x2()
{

}

bool MouseState::operator == ( const MouseState& other ) const
{
  return
  {
    x == other.x &&
    y == other.y &&
    left == other.left &&
    middle == other.middle &&
    right == other.right &&
    x1 == other.x1 &&
    x2 == other.x2
  };
}

MouseState::operator SDL_Point() const
{
  return { x, y };
}


Wrapper::Wrapper()
  : mInitialized()
  , mSoundInitialized()
  , mAllowWindowResize()
  , mWindowResized(false)
  , mUserDisplayIndex()
  , mUserScreenSize()
  , mUserWindowSize()
  , mWindow()
  , mRenderer()
  , mUserDisplayMode()
  , mSdlEvent()
  , mTextures({})
#ifdef SDL_TTF_SUPPORT
  , mFonts({})
#endif
#ifdef SDL_MIX_SUPPORT
  , mSounds({})
  , mMusic({})
#endif
{

}

Wrapper::~Wrapper()
{
  for ( const auto& texture : mTextures )
  {
    SDL_DestroyTexture( texture.second );
  }

#ifdef SDL_TTF_SUPPORT
  for ( const auto& font : mFonts )
  {
    TTF_CloseFont( font.second );
  }
#endif

#ifdef SDL_MIX_SUPPORT
  for ( const auto& sound : mSounds )
  {
    Mix_FreeChunk( sound.second );
  }
  for ( const auto& music : mMusic )
  {
    Mix_FreeMusic( music.second );
  }
#endif

  close();
}

void
Wrapper::logLibVer()
{
  SDL_version SDL2_libVer, SDL2_dllVer;
  SDL_VERSION( &SDL2_libVer );
  SDL_GetVersion( &SDL2_dllVer );

  std::string libVer, dllVer;
  libVer =  std::to_string( SDL2_libVer.major ) + "."
         +  std::to_string( SDL2_libVer.minor ) + "."
         +  std::to_string( SDL2_libVer.patch );

  dllVer =  std::to_string( SDL2_dllVer.major ) + "."
         +  std::to_string( SDL2_dllVer.minor ) + "."
         +  std::to_string( SDL2_dllVer.patch );

  LOG_INFO( "SDL2 lib ver: {}", libVer );
  LOG_INFO( "SDL2.dll ver: {}", dllVer );

#ifdef SDL_PNG_SUPPORT
  SDL_version SDL2_img_libVer;
  SDL_IMAGE_VERSION( &SDL2_img_libVer );
  const SDL_version* SDL2_img_dllVer = IMG_Linked_Version();

  libVer =  std::to_string( SDL2_img_libVer.major ) + "."
         +  std::to_string( SDL2_img_libVer.minor ) + "."
         +  std::to_string( SDL2_img_libVer.patch );

  dllVer =  std::to_string( SDL2_img_dllVer->major ) + "."
         +  std::to_string( SDL2_img_dllVer->minor ) + "."
         +  std::to_string( SDL2_img_dllVer->patch );

  LOG_INFO( "SDL2_image lib ver: {}", libVer );
  LOG_INFO( "SDL2_image.dll ver: {}", dllVer );
#else
  LOG_INFO( "SDL2_image was disabled for this application" );
#endif
#ifdef SDL_TTF_SUPPORT
  SDL_version SDL2_ttf_libVer;
  SDL_TTF_VERSION( &SDL2_ttf_libVer );
  const SDL_version* SDL2_ttf_dllVer = TTF_Linked_Version();

  libVer =  std::to_string( SDL2_ttf_libVer.major ) + "."
         +  std::to_string( SDL2_ttf_libVer.minor ) + "."
         +  std::to_string( SDL2_ttf_libVer.patch );

  dllVer =  std::to_string( SDL2_ttf_dllVer->major ) + "."
         +  std::to_string( SDL2_ttf_dllVer->minor ) + "."
         +  std::to_string( SDL2_ttf_dllVer->patch );

  LOG_INFO( "SDL2_ttf lib ver: {}", libVer );
  LOG_INFO( "SDL2_ttf.dll ver: {}", dllVer );
#else
  LOG_INFO( "SDL2_ttf was disabled for this application" );
#endif
#ifdef SDL_MIX_SUPPORT
  SDL_version SDL2_mix_libVer;
  SDL_MIXER_VERSION( &SDL2_mix_libVer );
  const SDL_version* SDL2_mix_dllVer = Mix_Linked_Version();

  libVer =  std::to_string( SDL2_mix_libVer.major ) + "."
         +  std::to_string( SDL2_mix_libVer.minor ) + "."
         +  std::to_string( SDL2_mix_libVer.patch );

  dllVer =  std::to_string( SDL2_mix_dllVer->major ) + "."
         +  std::to_string( SDL2_mix_dllVer->minor ) + "."
         +  std::to_string( SDL2_mix_dllVer->patch );

  LOG_INFO( "SDL2_mixer lib ver: {}", libVer );
  LOG_INFO( "SDL2_mixer.dll ver: {}", dllVer );
#else
  LOG_INFO( "SDL2_mixer was disabled for this application" );
#endif
}

bool
Wrapper::init(  SDL_Point           windowSize,
                const std::string&  windowTitle )
{
  if ( mInitialized )
    return true;

  logLibVer();

  LOG_INFO( "Initializing video subsystem" );
  if ( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 )
  {
    LOG_ERROR( "Failed to initialize SDL video subsystem: {}", SDL_GetError());
    showError(  "Error",
                "Failed to initialize video subsystem: "_s + SDL_GetError() );

    return false;
  }
  LOG_INFO( "Video subsystem initialized" );


// Set texture filtering to linear
  if ( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) )
    LOG_WARN( "Failed to enable linear texture filtering" );


// Disable Always-on-Top
  if ( !SDL_SetHint( SDL_HINT_ALLOW_TOPMOST, "0" ) )
    LOG_WARN( "Failed to disable Always-on-Top" );


  // Passing negative and zero window dimensions
  // creates window with desktop resolution
  // ( pseudo windowed borderless )
  if ( windowSize.x <= 0 || windowSize.y <= 0 )
    windowSize = getScreenResolution();


  LOG_INFO( "Creating window ({}x{})", windowSize.x, windowSize.y );
  mWindow = SDL_CreateWindow( windowTitle.c_str(),
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              windowSize.x,
                              windowSize.y,
                              SDL_WINDOW_SHOWN
//                              | SDL_WINDOW_RESIZABLE
                              );

  if ( mWindow == NULL )
  {
    LOG_ERROR( "Failed to create SDL window: {}", SDL_GetError());
    showError(  "Error",
                "Failed to create window: "_s + SDL_GetError() );

    return false;
  }
  LOG_INFO( "SDL INIT: SDL main window created" );


//  SDL_SetWindowPosition(  gWindow,
//                          userScreenWidth * 0.5f,
//                          userScreenHeight * 0.5f );
//
//  SDL_SetWindowMinimumSize( gWindow,
//                            userScreenWidth * 0.2f,
//                            userScreenHeight * 0.2f );
//
//  SDL_SetWindowMaximumSize( gWindow,
//                            userScreenWidth,
//                            userScreenHeight );


// Create renderer for window in accelerated mode
  mRenderer = SDL_CreateRenderer( mWindow,
                                  -1,
                                  SDL_RENDERER_ACCELERATED );

// If failed: create renderer in software mode
  if ( mRenderer == NULL )
  {
    LOG_ERROR( "Failed to create SDL renderer in accelerated mode: {}", SDL_GetError() );
    mRenderer = SDL_CreateRenderer( mWindow,
                                    -1,
                                    SDL_RENDERER_SOFTWARE );

    if ( mRenderer == NULL )
    {
      LOG_ERROR( "Failed to create SDL renderer in software mode: {}", SDL_GetError() );
      showError(  "Error",
                  "Failed to create renderer: "_s + SDL_GetError() );

      return false;
    }
  }
  LOG_INFO( "SDL INIT: Created SDL renderer for main window" );


// Initialize renderer color to black
  SDL_SetRenderDrawColor( mRenderer,
                          0x00, 0x00, 0x00, // R G B
                          0x00 );           // Alpha

#ifdef SDL_PNG_SUPPORT
  const int32_t imageFlags = IMG_INIT_PNG;  // | IMG_INIT_JPG
  const int32_t initResult = IMG_Init( imageFlags );

  if ( ( initResult & imageFlags ) != imageFlags )
  {
    LOG_ERROR( "Failed to initialize PNG support: {}", IMG_GetError() );
    showError( "Error",
                "Failed to initialize PNG support: "_s + IMG_GetError() );

    return false;
  }
  LOG_INFO( "SDL INIT: Initialized PNG support" );

#endif
#ifdef SDL_TTF_SUPPORT
  if ( TTF_Init() == -1 )
  {
    LOG_ERROR( "Failed to initialize TTF support: {}", TTF_GetError() );
    showError(  "Error",
                "Failed to initialize TTF support: "_s + TTF_GetError() );

    return false;
  }
  LOG_INFO( "SDL INIT: Initialized TTF support" );

#endif
#ifdef SDL_MIX_SUPPORT
  if ( SDL_InitSubSystem( SDL_INIT_AUDIO ) != 0 )
  {
    LOG_WARN( "Failed to initialize SDL audio subsystem: {}", SDL_GetError() );
    showWarning(  "Warning",
                  "Failed to initialize audio subsystem: "_s + SDL_GetError() );
  }
  else
    LOG_INFO( "SDL INIT: Initialized SDL audio subsystem" );

  if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) != 0 )
  {
    LOG_ERROR( "Failed to initialize SDL_mixer: ", Mix_GetError() );
    showWarning(  "Warning",
                  "Failed to initialize SDL_mixer: "_s + Mix_GetError() );
  }
  else
  {
    // Allocate 16 channels: initialization
    // allocates only 8 channels by default
    Mix_AllocateChannels( 16 );

    //  Reserve 2 channels from being used when
    //  trying to play sample at -1 channel
//    Mix_ReserveChannels( 2 );
    mSoundInitialized = true;

    LOG_INFO( "SDL INIT: Initialized SDL_mixer" );
  }
#endif

  mInitialized = true;
  updateScreenResolution( mUserDisplayIndex );
  updateWindowSize();

  LOG_INFO( "SDL INIT: Finished initializing" );

  return true;
}

void
Wrapper::close()
{
  if ( !mInitialized )
    return;

  SDL_DestroyRenderer( mRenderer );
  LOG_INFO( "SDL EXIT: Destroyed SDL renderer" );

  SDL_DestroyWindow( mWindow );
  LOG_INFO( "SDL EXIT: Destroyed SDL window" );

  mWindow   = NULL;
  mRenderer = NULL;

#ifdef SDL_MIX_SUPPORT
  // Call Mix_CloseAudio the same number of
  // Mix_OpenAudio calls to get it working
  Mix_CloseAudio();
  LOG_INFO( "SDL EXIT: Closed audio" );

  Mix_Quit();
  LOG_INFO( "SDL EXIT: Closed SDL_mixer" );

  SDL_QuitSubSystem( SDL_INIT_AUDIO );
#endif
#ifdef SDL_TTF_SUPPORT
  TTF_Quit();
  LOG_INFO( "SDL EXIT: Closed SDL_ttf" );
#endif
#ifdef SDL_PNG_SUPPORT
  IMG_Quit();
  LOG_INFO( "SDL EXIT: Closed SDL_image" );
#endif

  SDL_QuitSubSystem( SDL_INIT_VIDEO );
  LOG_INFO( "SDL EXIT: Closed SDL video subsystem" );

  SDL_Quit();
  LOG_INFO( "SDL EXIT: Closed SDL" );

  mSoundInitialized = false;
  mInitialized = false;

  return;
}

bool
Wrapper::grabMouseInputOn()
{
  if ( !mInitialized )
    return false;

  SDL_SetWindowGrab( mWindow, SDL_TRUE );

  SDL_WarpMouseInWindow(  mWindow,
                          windowSize().x * 0.5f,
                          windowSize().y * 0.5f );

  return true;
}

bool
Wrapper::grabMouseInputOff()
{
  if ( !mInitialized )
    return false;

  SDL_SetWindowGrab( mWindow, SDL_FALSE );

  return true;
}

bool
Wrapper::showError( const std::string& title,
                    const std::string& message ) const
{
  const int32_t result = SDL_ShowSimpleMessageBox(  SDL_MESSAGEBOX_ERROR,
                                                    title.c_str(),
                                                    message.c_str(),
                                                    NULL );

  if ( result < 0 )
  {
    LOG_ERROR( "Failed to show error window: {}", SDL_GetError() );
    return false;
  }

  return true;
}

bool
Wrapper::showWarning( const std::string& title,
                      const std::string& message ) const
{
  const int32_t result = SDL_ShowSimpleMessageBox(  SDL_MESSAGEBOX_WARNING,
                                                    title.c_str(),
                                                    message.c_str(),
                                                    NULL );

  if ( result < 0 )
  {
    LOG_WARN( "Failed to show warning window: {}", SDL_GetError() );
    return false;
  }

  return true;
}

Wrapper::IterTexture
Wrapper::loadTexture( const std::string& textureName,
                      const std::string& texturePath )
{
  if ( mRenderer == NULL || !mInitialized )
  {
    showError(  "Error",
                "Trying to load texture without renderer: " + texturePath );

    return {};
  }

#ifdef SDL_PNG_SUPPORT
  SDL_Surface* loadedSurface = IMG_Load( texturePath.c_str() );
#else
  SDL_Surface* loadedSurface = SDL_LoadBMP( texturePath.c_str() );
#endif

  if ( loadedSurface == NULL )
  {
    LOG_ERROR( "RESOURCES: Failed to load texture '{}': {}",
              texturePath,
#ifdef SDL_PNG_SUPPORT
              IMG_GetError()
#else
              SDL_GetError()
#endif
              );
    showError(  "Error",
                "Failed to load texture: " + texturePath );

    return {};
  }
  else
  {
    // Create texture from surface pixels
    SDL_Texture* newTexture =
      SDL_CreateTextureFromSurface( mRenderer,
                                    loadedSurface );

    if ( newTexture == NULL )
    {
      LOG_ERROR( "RESOURCES: Failed to create texture '{}' from surface: {}", texturePath, SDL_GetError() );
      showError(  "Error",
                  "Failed to create texture from surface: " + texturePath );
    }

    SDL_FreeSurface( loadedSurface );
    mTextures[textureName] = newTexture;

    return mTextures.find( textureName );
  }
}

#ifdef SDL_TTF_SUPPORT
Wrapper::IterFont
Wrapper::loadFont(  const std::string&  fontName,
                    const std::string&  fontPath,
                    const uint32_t      size,
                    const int32_t       index )
{
  TTF_Font* loadedFont =
    TTF_OpenFontIndex(  fontPath.c_str(),
                        size,
                        index );

  if ( loadedFont == NULL )
  {
    LOG_ERROR( "Failed to load font '{}':", TTF_GetError() );
    showError(  "Error",
                "Failed to load font: " + fontPath );

    return {};
  }

  mFonts[fontName] = loadedFont;

  return mFonts.find( fontName );
}

void
Wrapper::drawText(  const std::string&  text,
                    const SDL_Point     rect,
                    const TTF_Font*     font,
                    const float         size,
                    const SDL_Color     color ) const
{
  if ( !mInitialized )
    return;

  SDL_Surface* textSurface =
    TTF_RenderText_Solid( (TTF_Font*) font,
                          text.c_str(),
                          color );

  if ( textSurface == NULL )
  {
    SDL_FreeSurface( textSurface );
    return;
  }

  SDL_Texture* textTexture =
    SDL_CreateTextureFromSurface( mRenderer,
                                  textSurface );

  if ( textTexture == NULL )
  {
    SDL_DestroyTexture( textTexture );
    SDL_FreeSurface( textSurface );
    return;
  }

  const SDL_Point textSize = TextureSize( textTexture );

  SDL_Rect textRect = { rect.x, rect.y,
                        static_cast <int32_t> (textSize.x  * size),
                        static_cast <int32_t> (textSize.y  * size) };

  renderTexture(  textTexture,
                  textRect );

  SDL_DestroyTexture( textTexture );
  SDL_FreeSurface( textSurface );
}
#endif
#ifdef SDL_MIX_SUPPORT
Wrapper::IterSound
Wrapper::loadSound( const std::string& soundName,
                    const std::string& soundPath )
{
  if ( !mSoundInitialized )
  {
    LOG_ERROR( "Trying to load sound '{}' before audio initialization", soundPath );
    showError(  "Error",
                "Trying to load sound before audio initialization: " + soundPath );

    return {};
  }

  Mix_Chunk* loadedSound = Mix_LoadWAV( soundPath.c_str() );
  if ( loadedSound == NULL )
  {
    LOG_WARN( "Failed to load sound '{}': {}", soundPath, Mix_GetError() );
    showWarning(  "Error",
                  "Failed to load sound: " + soundPath );
  }

  mSounds[soundName] = loadedSound;

  return mSounds.find( soundName );
}

Wrapper::IterMusic
Wrapper::loadMusic( const std::string& musicName,
                    const std::string& musicPath )
{
  if ( !mSoundInitialized )
  {
    LOG_ERROR( "Trying to load music '{}' before audio initialization", musicPath );
    showError(  "Error",
                "Trying to load music before audio initialization: " + musicPath );

    return {};
  }

  Mix_Music* loadedMusic = Mix_LoadMUS( musicPath.c_str() );
  if ( loadedMusic == NULL )
  {
    LOG_WARN( "Failed to load music '{}': {}", musicPath, Mix_GetError() );
    showWarning(  "Error",
                  "Failed to load music: " + musicPath );
  }

  mMusic[musicName] = loadedMusic;

  return mMusic.find( musicName );
}
#endif

void
Wrapper::renderTexture( const SDL_Texture*      texture,
                        const SDL_Rect&         dstRect,
                        const SDL_Rect*         srcRect,
                        const double            angle,
                        const SDL_Point*        center,
                        const SDL_RendererFlip  flip ) const
{
  if ( !mInitialized )
    return;

  SDL_RenderCopyEx( mRenderer,
                    (SDL_Texture*) texture,
                    srcRect, &dstRect,
                    angle, center,
                    flip );
}

SDL_Point
Wrapper::getScreenResolution( const int32_t displayIndex )
{
  SDL_DisplayMode displayMode = {};
  SDL_GetDesktopDisplayMode(  displayIndex,
                              &displayMode );

  return SDL_Point
  {
    displayMode.w,
    displayMode.h,
  };
}

void
Wrapper::updateScreenResolution( const int32_t displayIndex )
{
  mUserScreenSize = getScreenResolution( displayIndex );
}

#ifdef SDL_MIX_SUPPORT
int32_t
Wrapper::soundPlay( const Mix_Chunk*  sound,
                    const int32_t     channel,
                    const int32_t     loopCount ) const
{
//  if ( !Mix_Playing( channel ) )
  return soundPlayFadeIn( (Mix_Chunk*) sound,
                          0,
                          channel,
                          loopCount );
}

int32_t
Wrapper::soundPlayFadeIn( const Mix_Chunk*  sound,
                          const uint32_t    fadeInMs,
                          const int32_t     channel,
                          const int32_t     loopCount ) const
{
  if ( !mSoundInitialized )
    return -1;

  return Mix_FadeInChannel( channel,
                            (Mix_Chunk*) sound,
                            loopCount,
                            fadeInMs );
}

int32_t
Wrapper::soundStop( const int32_t channel ) const
{
  return soundStopFadeOut(  channel,
                            0 );
}

int32_t
Wrapper::soundStopFadeOut(  const int32_t   channel,
                            const uint32_t  fadeOutMs ) const
{
  if ( !mSoundInitialized )
    return -1;

  return Mix_FadeOutChannel(  channel,
                              fadeOutMs );
}

int32_t
Wrapper::soundStopAllFadeOut( const uint32_t fadeOutMs ) const
{
  return soundStopFadeOut(  -1,
                            fadeOutMs );
}

int32_t
Wrapper::soundStopAll() const
{
  return soundStop( -1 );
}

void
Wrapper::soundPause( const int32_t channel ) const
{
  if ( !mSoundInitialized )
    return;

  Mix_Pause( channel );
}

void
Wrapper::soundPauseAll() const
{
  soundPause( -1 );
}

void
Wrapper::soundResume( const int32_t channel ) const
{
  if ( !mSoundInitialized )
    return;

  Mix_Resume( channel );
}

void
Wrapper::soundResumeAll() const
{
  soundResume( -1 );
}
#endif

bool
Wrapper::updateEvents()
{
  bool result = SDL_PollEvent( &mSdlEvent );
  if ( !result )
    return result;

  if ( mSdlEvent.type == SDL_WINDOWEVENT )
    if ( mSdlEvent.window.type == SDL_WINDOWEVENT_SIZE_CHANGED )
      mWindowResized = true;

  return result;
}

void
Wrapper::renderUpdate() const
{
  if ( !mInitialized )
    return;

  SDL_RenderPresent( mRenderer );
  SDL_RenderClear( mRenderer );
}

SDL_Point
Wrapper::TextureSize( const SDL_Texture* texture )
{
  SDL_Point textureSize;
  SDL_QueryTexture( (SDL_Texture*) texture,
                    NULL, NULL,
                    &textureSize.x, &textureSize.y );

  return SDL_Point
  {
    textureSize.x,
    textureSize.y
  };
}

bool
Wrapper::PointInRect( SDL_Point point,
                      SDL_Rect  rect )
{
  return
  {
      point.x > rect.x &&
      point.y > rect.y &&
      point.x < rect.x + rect.w &&
      point.y < rect.y + rect.h
  };
}

bool
Wrapper::setWindowTitle( const std::string& title ) const
{
  if ( !mInitialized )
    return false;

  SDL_SetWindowTitle( mWindow,
                      title.c_str() );

  return true;
}

bool
Wrapper::windowResized()
{
  if ( mWindowResized )
  {
    mWindowResized = false;
    return true;
  }

  return false;
}

SDL_Point
Wrapper::updateWindowSize()
{
  if ( !mInitialized )
    return SDL_Point();

  SDL_GetRendererOutputSize(  mRenderer,
                              &mUserWindowSize.x,
                              &mUserWindowSize.y );

  return SDL_Point
  {
    mUserWindowSize.x,
    mUserWindowSize.y
  };
}

SDL_Point
Wrapper::windowSize() const
{
  return SDL_Point
  {
    mUserWindowSize.x,
    mUserWindowSize.y
  };
}

int32_t
Wrapper::windowHeight() const
{
  return windowSize().y;
}

int32_t
Wrapper::windowWidth() const
{
  return windowSize().x;
}

int32_t
Wrapper::screenHeight() const
{
  return mUserScreenSize.y;
}

int32_t
Wrapper::screenWidth() const
{
  return mUserScreenSize.x;
}

MouseState
Wrapper::mouseState() const
{
  if ( !mInitialized )
    return {};

  MouseState state;
  Uint32 bitMask = SDL_GetMouseState( &state.x,
                                      &state.y );

  state.left = bitMask & SDL_BUTTON(SDL_BUTTON_LEFT);
  state.middle = bitMask & SDL_BUTTON(SDL_BUTTON_MIDDLE);
  state.right = bitMask & SDL_BUTTON(SDL_BUTTON_RIGHT);
  state.x1 = bitMask & SDL_BUTTON(SDL_BUTTON_X1);
  state.x2 = bitMask & SDL_BUTTON(SDL_BUTTON_X2);

  return state;
}

SDL_Event
Wrapper::event() const
{
  return mSdlEvent;
}

SDL_Window*
Wrapper::window() const
{
  return mWindow;
}

SDL_Renderer*
Wrapper::renderer() const
{
  return mRenderer;
}

bool
Wrapper::isInitialized() const
{
  return mInitialized;
}

bool
Wrapper::isSoundInitialized() const
{
  return mSoundInitialized;
}

Wrapper::IterTexture
Wrapper::texture( const std::string& textureName ) const
{
  auto texture = mTextures.find( textureName );
  if ( texture != mTextures.end() )
    return texture;

  LOG_ERROR( "RESOURCES: Trying to use nonexistent texture '{}'", textureName );
  showError(  "Error",
              "Trying to use nonexistent texture: " + textureName );

  return {};
}

#ifdef SDL_TTF_SUPPORT
Wrapper::IterFont
Wrapper::font( const std::string& fontName ) const
{
  auto font = mFonts.find( fontName );
  if ( font != mFonts.end() )
    return font;

  LOG_ERROR( "RESOURCES: Trying to use nonexistent font '{}'", fontName );
  showError(  "Error",
              "Trying to use nonexistent font: " + fontName );

  return {};
}
#endif

#ifdef SDL_MIX_SUPPORT
Wrapper::IterSound
Wrapper::sound( const std::string& soundName ) const
{
  auto sound = mSounds.find( soundName );
  if ( sound != mSounds.end() )
    return sound;

  LOG_WARN( "RESOURCES: Trying to use nonexistent sound '{}'", soundName );
  showWarning(  "Warning",
                "Trying to use nonexistent sound: " + soundName );

  return {};
}

Wrapper::IterMusic
Wrapper::music( const std::string& musicName ) const
{
  auto music = mMusic.find( musicName );
  if ( music != mMusic.end() )
    return music;

  LOG_WARN( "RESOURCES: Trying to use nonexistent music '{}'", musicName );
  showWarning(  "Warning",
                "Trying to use nonexistent music: " + musicName );

  return {};
}
#endif

} // namespace SDL2
