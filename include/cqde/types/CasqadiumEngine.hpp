#pragma once

#include <cqde/types/ConfigManager.hpp>
#include <cqde/types/LaunchParams.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entity/registry.hpp>

#include <glm/vec2.hpp>

#include <spdlog/spdlog.h>


struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;

struct ImGuiContext;


namespace cqde::types
{

class CasqadiumEngine
{
public:
  using WindowHints = std::unordered_map <int32_t, int32_t>;
  using Duration = TimeUtils::Duration;


private:
  entt::registry mRegistry {};

  LaunchParams mLaunchParams {};
  std::shared_ptr <Config> mConfig {std::make_shared <Config> ()};

  std::string mWindowTitle {};

  Duration mDeltaTime {};
  Duration mTickInterval {};
  Duration mFrameInterval {};

  glm::dvec2 mMousePos {};

  glm::ivec2 mWindowSize {};
  glm::ivec2 mFramebufferSize {};

  double mFramerate {};

  GLFWwindow* mWindow {};
  GLFWwindow* mFocusedWindow {};
  GLFWwindow* mHoveredWindow {};


  bool calcVideoMode(
    GLFWmonitor*,
    GLFWvidmode& ) const;


  void setupEventCallbacks();

  bool initAudio();
  void initCommon();
  void initPhysics();
  bool loadShaders();

  void registerComponents();
  void registerCallbacks();
  void registerSystems();
  void registerSequenceSteps();


  void update(
    const uint32_t ticks,
    const Duration& interval );

  void render(
    const uint32_t frames,
    const Duration& interval );


protected:

//  Window events
  void windowFocusEvent(
    GLFWwindow*,
    int32_t focused );

  void framebufferSizeEvent(
    GLFWwindow*,
    int32_t width,
    int32_t height );

  void windowSizeEvent(
    GLFWwindow*,
    int32_t width,
    int32_t height );

  void dragDropEvent(
    GLFWwindow*,
    int32_t pathCount,
    const char* paths[] );


//  Input events
  void keyEvent(
    GLFWwindow*,
    int32_t key,
    int32_t scancode,
    int32_t action,
    int32_t mods );

  void textEvent(
    GLFWwindow*,
    uint32_t codepoint );

  void mouseButtonEvent(
    GLFWwindow*,
    int32_t button,
    int32_t action,
    int32_t mods );

  void mousePosEvent(
    GLFWwindow*,
    double xpos,
    double ypos );

  void mouseEnterEvent(
    GLFWwindow*,
    int32_t entered );

  void mouseWheelEvent(
    GLFWwindow*,
    double xoffset,
    double yoffset );


public:
  CasqadiumEngine() = default;
  CasqadiumEngine( const LaunchParams& );

  void readConfig() const;

  bool init(
    const WindowHints& = {} );

  void shutdown();

  void run();


  std::shared_ptr <spdlog::logger> logger() const;

  void setConfig( const std::shared_ptr <Config> );
  void setLaunchParams( const LaunchParams& );
  void setWindowTitle( const std::string& );

  void setTickInterval( const Duration& );
  void setFrameInterval( const Duration& );

  std::shared_ptr <Config> config() const;
  LaunchParams launchParams() const;
  std::string windowTitle() const;

  Duration deltaTime() const;
  Duration tickInterval() const;
  Duration frameInterval() const;

  entt::registry& registry();
  const entt::registry& registry() const;

  GLFWwindow* window() const;

  glm::ivec2 windowSize() const;
  glm::ivec2 framebufferSize() const;

  glm::dvec2 mousePos() const;

  double framerate() const;

  GLFWwindow* focusedWindow() const;
  GLFWwindow* hoveredWindow() const;

  static ImGuiContext* ImguiContext();
};

} // namespace cqde::types
