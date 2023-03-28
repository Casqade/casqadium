#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/logger.hpp>
#include <cqde/static_strings.hpp>

#include <cqde/callbacks/editor_common.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/ConfigManager.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/PrefabManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/graphics/FrameReadback.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/MouseCursorAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <olcPGE/Font.hpp>

#include <ctpl/ctpl_stl.h>

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


namespace cqde::types
{

static CasqadiumEngine::WindowHints
windowHintsDefault()
{
  return
  {
    {GLFW_RESIZABLE, GLFW_TRUE},
    {GLFW_VISIBLE, GLFW_TRUE},
    {GLFW_DECORATED, GLFW_TRUE},
    {GLFW_FOCUSED, GLFW_TRUE},
    {GLFW_AUTO_ICONIFY, GLFW_TRUE},
    {GLFW_FLOATING, GLFW_FALSE},
    {GLFW_MAXIMIZED, GLFW_FALSE},
    {GLFW_CENTER_CURSOR, GLFW_TRUE},
    {GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE},
    {GLFW_FOCUS_ON_SHOW, GLFW_TRUE},
    {GLFW_SCALE_TO_MONITOR, GLFW_FALSE},

    {GLFW_RED_BITS, 8},
    {GLFW_GREEN_BITS, 8},
    {GLFW_BLUE_BITS, 8},
    {GLFW_ALPHA_BITS, 8},

    {GLFW_DEPTH_BITS, 24},
    {GLFW_STENCIL_BITS, 8},

    {GLFW_SAMPLES, 0},

    {GLFW_REFRESH_RATE, GLFW_DONT_CARE},
    {GLFW_STEREO, GLFW_FALSE},
    {GLFW_SRGB_CAPABLE, GLFW_FALSE},
    {GLFW_DOUBLEBUFFER, GLFW_TRUE},
    {GLFW_CLIENT_API, GLFW_OPENGL_API},
    {GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API},
    {GLFW_CONTEXT_VERSION_MAJOR, 1},
    {GLFW_CONTEXT_VERSION_MINOR, 0},
    {GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_ROBUSTNESS},
    {GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_ANY_RELEASE_BEHAVIOR},
    {GLFW_CONTEXT_NO_ERROR, GLFW_FALSE},
    {GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE},
    {GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE},
    {GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE},
  };
}

static void APIENTRY glDebugCallback(
  GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userData )
{
  const auto engine = static_cast <const CasqadiumEngine*> (userData);

  using spdlog::level::level_enum;

  std::string sourceString {};

  level_enum level {};

  switch (source)
  {
    case GL_DEBUG_SOURCE_API:
      sourceString = "API";
      break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      sourceString = "Window";
      break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      sourceString = "Compiler";
      break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
      sourceString = "App";
      break;

    case GL_DEBUG_SOURCE_APPLICATION:
      sourceString = "User";
      break;

    case GL_DEBUG_SOURCE_OTHER:
      sourceString = "Other";
      break;

    default:
      sourceString = "Unknown";
      break;
  }

  switch (type)
  {
    case GL_DEBUG_TYPE_ERROR:
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      level = level_enum::err;
      break;

    case GL_DEBUG_TYPE_PORTABILITY:
    case GL_DEBUG_TYPE_PERFORMANCE:
      level = level_enum::warn;
      break;

    default:
      level = level_enum::trace;
      break;
  }

  spdlog::log(level, "[OpenGL: {}] {}", sourceString, message);

  CQDE_ASSERT_DEBUG(level < level_enum::err, return);
}

CasqadiumEngine::CasqadiumEngine(
  const LaunchParams& launchParams )
  : mLaunchParams{launchParams}
{}

void
CasqadiumEngine::readConfig() const
{
  const auto executableName = mLaunchParams.executableName();
  const auto configFilename = executableName + ".conf";

  ConfigManager configManager {configFilename};

  configManager.setConfig(mConfig);

  if ( fileExists(configFilename) == true )
    configManager.read();

  else
    configManager.write();
}

bool
CasqadiumEngine::calcVideoMode(
  GLFWmonitor* monitor,
  GLFWvidmode& videoMode ) const
{
  auto& windowWidth = mConfig->video.width;
  auto& windowHeight = mConfig->video.height;

  auto& monitorIndex = mConfig->video.screen;

  monitor = glfwGetPrimaryMonitor();

  if ( monitorIndex >= 0 &&
       mConfig->video.fullscreen == true )
  {
    int32_t monitorCount;
    const auto monitors = glfwGetMonitors(&monitorCount);

    if ( monitors == nullptr )
    {
      LOG_CRITICAL("GLFW: Failed to query monitors info");
      return false;
    }

    LOG_DEBUG("GLFW: Found {} monitor(s)", monitorCount);

    if ( monitorIndex >= monitorCount )
      monitorIndex = 0;

    monitor = monitors[monitorIndex];
  }

  auto currentMonitorMode = glfwGetVideoMode(monitor);

  if ( currentMonitorMode == nullptr )
  {
    LOG_CRITICAL("GLFW: Failed to query current video mode for monitor {} ({})",
                  monitorIndex, glfwGetMonitorName(monitor));
    return false;
  }

  bool useScreenResolution {true};

  if ( mConfig->video.fullscreen == true )
  {
    int32_t monitorModeCount;
    auto currentMonitorModes = glfwGetVideoModes(
      monitor,
      &monitorModeCount );

    if ( currentMonitorModes == nullptr )
    {
      LOG_CRITICAL("GLFW: Failed to query available video modes for monitor {} ({})",
                    monitorIndex, glfwGetMonitorName(monitor));
      return false;
    }

    for ( int32_t i = monitorModeCount - 1; i >= 0; --i )
    {
      const auto monitorMode = currentMonitorModes[i];

      if ( windowWidth == monitorMode.width &&
           windowHeight == monitorMode.height )
      {
        useScreenResolution = false;
        break;
      }
    }
  }
  else if ( mConfig->video.borderless == false )
  {
    const Config defaultConfig {};

    const auto defaultWidth = defaultConfig.video.width;
    const auto defaultHeight = defaultConfig.video.height;

    windowWidth =
      std::min( std::max(defaultWidth, windowWidth),
                currentMonitorMode->width );

    windowHeight =
      std::min( std::max(defaultHeight, windowHeight),
                currentMonitorMode->height );

    useScreenResolution = false;
  }

  if ( useScreenResolution == true )
  {
    windowWidth = currentMonitorMode->width;
    windowHeight = currentMonitorMode->height;
  }

  videoMode.width = windowWidth;
  videoMode.height = windowHeight;

  videoMode.redBits = currentMonitorMode->redBits;
  videoMode.greenBits = currentMonitorMode->greenBits;
  videoMode.blueBits = currentMonitorMode->blueBits;
  videoMode.refreshRate = currentMonitorMode->refreshRate;

  if ( mConfig->video.fullscreen == false )
    monitor = nullptr;

  return true;
}

void
CasqadiumEngine::initCommon()
{
  const auto maxThreadCount =
    std::max(1u, std::thread::hardware_concurrency());

  auto& tp = mRegistry.ctx().emplace <ctpl::thread_pool> (1);

  mRegistry.ctx().emplace <EntityManager> ();
  mRegistry.ctx().emplace <CallbackManager> ();
  mRegistry.ctx().emplace <InputManager> ();
  mRegistry.ctx().emplace <PackageManager> ();
  mRegistry.ctx().emplace <PrefabManager> ();
  mRegistry.ctx().emplace <SystemManager> ();
  mRegistry.ctx().emplace <SnapshotManager> ();
  mRegistry.ctx().emplace <UserManager> ();
  mRegistry.ctx().emplace <FrameReadbackQueue> ();

  mRegistry.ctx().emplace <AudioAssetManager> (tp);
  mRegistry.ctx().emplace <FontAssetManager> (tp);
  mRegistry.ctx().emplace <GeometryAssetManager> (tp);
  mRegistry.ctx().emplace <MouseCursorAssetManager> (tp);
  mRegistry.ctx().emplace <TerrainAssetManager> (tp);
  mRegistry.ctx().emplace <TextureAssetManager> (tp);
  mRegistry.ctx().emplace <TextStringAssetManager> (tp);

  registerComponents();
  registerCallbacks();
  registerSystems();
  registerSequenceSteps();
}

bool
CasqadiumEngine::init(
  const WindowHints& windowHints )
{
  CQDE_ASSERT_DEBUG(mWindow == nullptr, return false);
  CQDE_ASSERT_DEBUG(mConfig != nullptr, return false);

  logger();

  const auto executableName = mLaunchParams.executableName();

//  olc::Font
  LOG_DEBUG("Initializing olc::Font");

  const auto olcFontInitStatus = olc::Font::Init();

  if ( olcFontInitStatus.size() != 0 )
  {
    LOG_ERROR("Failed to initialize olc::Font: {}",
              olcFontInitStatus);
    return false;
  }

  glfwSetErrorCallback(
  [] ( const int errorCode, const char* message )
  {
    LOG_ERROR("GLFW: error code {}: {}",
      errorCode,
      message != nullptr
              ? message
              : "No description provided");
  });

  glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);


//  GLFW

  LOG_DEBUG("Initializing GLFW");

  if ( glfwInit() != GLFW_TRUE )
  {
    LOG_ERROR("Failed to initialize GLFW");
    shutdown();
    return false;
  }

  glfwSetMonitorCallback(
  [] ( GLFWmonitor* monitor, int event )
  {
    if ( event == GLFW_DISCONNECTED )
      LOG_WARN("GLFW: Monitor '{}' was disconnected",
                glfwGetMonitorName(monitor));
  });

  GLFWmonitor* monitor {};
  GLFWvidmode videoMode {};

  if ( calcVideoMode(monitor, videoMode) == false )
  {
    shutdown();
    return false;
  }

  ConfigManager configManager {executableName + ".conf"};
  configManager.setConfig(mConfig);
  configManager.write();

  auto hints = windowHintsDefault();

  hints[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_CORE_PROFILE;
  hints[GLFW_RED_BITS] = videoMode.redBits;
  hints[GLFW_GREEN_BITS] = videoMode.greenBits;
  hints[GLFW_BLUE_BITS] = videoMode.blueBits;
  hints[GLFW_REFRESH_RATE] = videoMode.refreshRate;

  for ( const auto& [hint, value] : windowHints )
    hints[hint] = value;

  if ( mConfig->video.borderless == true )
    hints[GLFW_DECORATED] = false;

  glfwDefaultWindowHints();

  for ( const auto& [hint, value] : windowHints )
    glfwWindowHint(hint, value);

  if ( mWindowTitle.empty() == true )
    mWindowTitle = executableName;

//  GLFW window

  LOG_DEBUG("Creating GLFW window");

  mWindow = glfwCreateWindow(
    videoMode.width, videoMode.height,
    mWindowTitle.c_str(),
    monitor, NULL );

  if ( mWindow == nullptr )
  {
    LOG_CRITICAL("GLFW: Failed to create window");
    shutdown();
    return false;
  }

  glfwSetWindowUserPointer(mWindow, this);

  glfwMakeContextCurrent(mWindow);
  glfwSwapInterval(mConfig->video.vsync);

  setupEventCallbacks();

//  GLAD

  LOG_DEBUG("Initializing GLAD");

  if ( gladLoadGL(glfwGetProcAddress) == false )
  {
    LOG_CRITICAL("Failed to initialize GLAD");
    shutdown();
    return false;
  }

//  todo: config+launchparam entries
  glDebugMessageControl(
    GL_DONT_CARE,
    GL_DONT_CARE,
    GL_DEBUG_SEVERITY_NOTIFICATION,
    0, nullptr,
    GL_FALSE );

  glDebugMessageControl(
    GL_DEBUG_SOURCE_SHADER_COMPILER,
    GL_DEBUG_TYPE_ERROR,
    GL_DONT_CARE,
    0, nullptr,
    GL_FALSE );

  glDebugMessageCallback(glDebugCallback, this);
  glEnable(GL_DEBUG_OUTPUT);

  const auto tickRate = mConfig->engine.tickRate;
  const auto frameRate = mConfig->engine.frameRate;

  if ( tickRate != 0 )
    mTickInterval = 1.0 / tickRate;

  if ( frameRate != 0 )
    mFrameInterval = 1.0 / frameRate;


//  Dear ImGui

  LOG_DEBUG("Initializing ImGui");

  static auto imguiIniFileName = executableName + ".imgui";
  imguiIniFileName = executableName + ".imgui";

  ImGuizmo::SetImGuiContext(ImGui::CreateContext());

  ImGuiIO& io = ImGui::GetIO();

//  io.ConfigDockingWithShift = true;
  io.ConfigWindowsMoveFromTitleBarOnly = true;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
//  io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
  io.IniFilename = imguiIniFileName.data();
  io.UserData = this;

  ImGui::LoadIniSettingsFromMemory(EditorImGuiSettings());
  ImGui::LoadIniSettingsFromDisk(io.IniFilename);

  if ( ImGui_ImplGlfw_InitForOpenGL(mWindow, true) == false )
  {
    LOG_ERROR("Failed to initialize DearImGui GLFW backend");
    shutdown();
    return false;
  }
  ImGui_ImplGlfw_SetCallbacksChainForAllWindows(true);

  if ( ImGui_ImplOpenGL3_Init() == false )
  {
    LOG_ERROR("Failed to initialize DearImGui OpenGL3 backend");
    shutdown();
    return false;
  }

//  Engine

  mRegistry.ctx().emplace <CasqadiumEngine*> (this);
  mRegistry.ctx().emplace <ConfigManager> (configManager);
  mRegistry.ctx().emplace <LaunchParams> (mLaunchParams);

  if ( loadShaders() == false )
  {
    shutdown();
    return false;
  }

  initAudio();
  initPhysics();
  initCommon();

  if ( glfwGetWindowAttrib(mWindow, GLFW_FOCUSED) == true )
    mFocusedWindow = mWindow;

  if ( glfwGetWindowAttrib(mWindow, GLFW_HOVERED) == true )
    mHoveredWindow = mWindow;

  glfwGetCursorPos(
    mWindow,
    &mMousePos.x,
    &mMousePos.y );

  glfwGetWindowSize(
    mWindow,
    &mWindowSize.x,
    &mWindowSize.y );

  glfwGetFramebufferSize(
    mWindow,
    &mFramebufferSize.x,
    &mFramebufferSize.y );

  return true;
}

void
CasqadiumEngine::shutdown()
{
  mRegistry.clear();

//  audio must be killed before SoLoud destruction
  mRegistry.ctx().erase <AudioAssetManager> ();
  mRegistry.ctx() = {};

  const auto olcFontDeinitStatus = olc::Font::Deinit();

  if ( olcFontDeinitStatus.size() != 0 )
    LOG_WARN("Failed to deinitialize olc::Font: {}",
              olcFontDeinitStatus);

  if ( ImGui::GetCurrentContext() != nullptr )
  {
    LOG_TRACE("Terminating ImGui");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  LOG_TRACE("Destroying main window");
  glfwDestroyWindow(mWindow);

  LOG_TRACE("Terminating GLFW");
  glfwTerminate();

  mWindow = nullptr;
}

void
CasqadiumEngine::run()
{
  using fmt::format;
  using callbacks::editorModeEnable;

  CQDE_ASSERT_DEBUG(mWindow != nullptr, return);

  auto& packageManager = mRegistry.ctx().get <PackageManager> ();
  packageManager.load(mRegistry);

  auto& userManager = mRegistry.ctx().get <UserManager> ();
  userManager.setUser(mConfig->lastUser);

  auto& inputManager = mRegistry.ctx().get <InputManager> ();
  inputManager.load(userManager.inputConfigPath());

  if ( mLaunchParams.startEditor() == true )
    editorModeEnable(mRegistry);
  else
  {
    auto& entityManager = mRegistry.ctx().get <EntityManager> ();
    entityManager.entryPointExecute(mRegistry);
  }

  TimeUtils::TimePeriodInit();

  mRegistry.ctx().emplace <TickCurrent> ();
  mRegistry.ctx().emplace <FrameCurrent> ();

  bool worldStateUpdated {};

  auto timePrevious = TimeUtils::Now();
  auto tickPrevious = timePrevious;
  auto framePrevious = tickPrevious;

  Duration frameTimer {};
  uint32_t frameCount {};

  while ( glfwWindowShouldClose(mWindow) == false )
  {
    const bool tickRateLimited = mTickInterval > Duration {};
    const bool frameRateLimited = mFrameInterval > Duration {};

    const auto tickTimeToSleep = tickPrevious + mTickInterval;
    const auto frameTimeToSleep = framePrevious + mFrameInterval;

    TimeUtils::SleepUntil( std::min(tickTimeToSleep, frameTimeToSleep) );

    const auto currentTime = TimeUtils::Now();
    mDeltaTime = currentTime - timePrevious;
    timePrevious = currentTime;

    glfwPollEvents();
    inputManager.updateInput(mRegistry);

    uint32_t ticks = 0;

    if ( tickRateLimited == true )
      for ( ; currentTime >= tickPrevious + mTickInterval;
              tickPrevious += mTickInterval )
        ++ticks;
    else
    {
      ticks = 1;
      mTickInterval = currentTime - tickPrevious;
      tickPrevious = currentTime;
    }

    if ( ticks != 0 )
    {
      update(ticks, mTickInterval);
      worldStateUpdated = true;
    }

    uint32_t frames = 0;

    if ( frameRateLimited == true )
      for ( ; currentTime >= framePrevious + mFrameInterval;
              framePrevious += mFrameInterval )
        ++frames;
    else
      frames = 1;

    if ( frames != 0 &&
         (frameRateLimited == false || worldStateUpdated == true) )
    {
      render(frames, mFrameInterval);
      worldStateUpdated = false;
      ++frameCount;
    }

    if ( currentTime >= frameTimer + Duration{1} )
    {
      size_t seconds {};
      for ( ; currentTime >= frameTimer + Duration{1};
            ++seconds )
        frameTimer += Duration{1};

      mFramerate = static_cast <double> (frameCount) / seconds;
      frameCount = 0;

      const auto title = format(
        "{} - FPS: {:.2f} ({:.2f} ms)",
        mWindowTitle,
        mFramerate,
        1000.0 / mFramerate );

      glfwSetWindowTitle(
        mWindow,
        title.c_str() );
    }
  }

  TimeUtils::TimePeriodDeinit();
}

void
CasqadiumEngine::update(
  const uint32_t ticks,
  const Duration& interval )
{
  auto& callbackManager = mRegistry.ctx().get <CallbackManager> ();
  auto& entityManager = mRegistry.ctx().get <EntityManager> ();

  auto& tick = mRegistry.ctx().get <TickCurrent> ();

  tick.ticksElapsed = ticks;
  tick.tickInterval = interval;

  for ( uint32_t i = 0; i < ticks; ++i )
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    mRegistry.ctx().get <SystemManager> ().execute(
      mRegistry, System::Phase::Logic );

    callbackManager.delayedExecution(mRegistry);
    entityManager.delayedRemove(mRegistry);

    ImGui::Render();
    ImGui::UpdatePlatformWindows();

    tick.lastTickTimepoint = TimeUtils::Now();
  }
}

void
CasqadiumEngine::render(
  const uint32_t frames,
  const Duration& interval )
{
  auto& frame = mRegistry.ctx().get <FrameCurrent> ();

  frame.framesElapsed = frames;
  frame.frameInterval = interval;

  glfwMakeContextCurrent(mWindow);

  mRegistry.ctx().get <SystemManager> ().execute(
    mRegistry, System::Phase::Render );

  frame.lastFrameTimepoint = TimeUtils::Now();

  const auto drawData = ImGui::GetDrawData();
  if ( drawData != nullptr )
  {
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
    ImGui::RenderPlatformWindowsDefault();
  }

  glfwMakeContextCurrent(mWindow);
  glfwSwapBuffers(mWindow);
}

void
CasqadiumEngine::setConfig(
  const std::shared_ptr <Config> config )
{
  mConfig = std::move(config);
}

void
CasqadiumEngine::setLaunchParams(
  const LaunchParams& launchParams )
{
  mLaunchParams = launchParams;
}

void
CasqadiumEngine::setWindowTitle(
  const std::string& title )
{
  mWindowTitle = title;
}

void
CasqadiumEngine::setTickInterval(
  const Duration& tickInterval )
{
  mTickInterval = tickInterval;
}

void
CasqadiumEngine::setFrameInterval(
  const Duration& frameInterval )
{
  mFrameInterval = frameInterval;
}

std::shared_ptr <Config>
CasqadiumEngine::config() const
{
  return mConfig;
}

LaunchParams
CasqadiumEngine::launchParams() const
{
  return mLaunchParams;
}

std::string
CasqadiumEngine::windowTitle() const
{
  return mWindowTitle;
}

TimeUtils::Duration
CasqadiumEngine::deltaTime() const
{
  return mDeltaTime;
}

TimeUtils::Duration
CasqadiumEngine::tickInterval() const
{
  return mTickInterval;
}

TimeUtils::Duration
CasqadiumEngine::frameInterval() const
{
  return mFrameInterval;
}

entt::registry&
CasqadiumEngine::registry()
{
  return mRegistry;
}

const entt::registry&
CasqadiumEngine::registry() const
{
  return mRegistry;
}

GLFWwindow*
CasqadiumEngine::window() const
{
  return mWindow;
}

glm::ivec2
CasqadiumEngine::windowSize() const
{
  return mWindowSize;
}

glm::ivec2
CasqadiumEngine::framebufferSize() const
{
  return mFramebufferSize;
}

glm::dvec2
CasqadiumEngine::mousePos() const
{
  return mMousePos;
}

double
CasqadiumEngine::framerate() const
{
  return mFramerate;
}

GLFWwindow*
CasqadiumEngine::focusedWindow() const
{
  return mFocusedWindow;
}

GLFWwindow*
CasqadiumEngine::hoveredWindow() const
{
  return mHoveredWindow;
}

ImGuiContext*
CasqadiumEngine::ImguiContext()
{
  const auto context = ImGui::GetCurrentContext();
  CQDE_ASSERT_DEBUG(context != nullptr, return nullptr);
  return context;
}

} // namespace cqde::types
