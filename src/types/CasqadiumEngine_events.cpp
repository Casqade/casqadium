#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/SnapshotManager.hpp>

#include <cqde/logger.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/MouseInputId.hpp>

#include <GLFW/glfw3.h>

#include <imgui.h>


namespace cqde::types
{

void
CasqadiumEngine::windowFocusEvent(
  GLFWwindow* window,
  int32_t focused )
{
  if ( focused == true )
    mFocusedWindow = window;

  else if ( mFocusedWindow == window )
    mFocusedWindow = nullptr;
}

void
CasqadiumEngine::framebufferSizeEvent(
  GLFWwindow* window,
  int32_t width,
  int32_t height )
{
  LOG_INFO("{} fb size: {}x{}", fmt::ptr(window), width, height);
  mFramebufferSize = {width, height};
}

void
CasqadiumEngine::windowSizeEvent(
  GLFWwindow* window,
  int32_t width,
  int32_t height )
{
  mWindowSize = {width, height};
}

void
CasqadiumEngine::dragDropEvent(
  GLFWwindow* window,
  int32_t pathCount,
  const char* paths[] )
{
  if ( pathCount != 1 )
    return;

  try
  {
    SnapshotManager::Load(mRegistry, paths[0]);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR(e.what());
  }
}

void
CasqadiumEngine::keyEvent(
  GLFWwindow*,
  int32_t key,
  int32_t scancode,
  int32_t action,
  int32_t mods )
{
  if ( action == GLFW_REPEAT )
    return;

  const auto& io = ImGui::GetIO();

  if ( io.WantCaptureKeyboard == true ||
       io.WantTextInput == true )
  {
    const auto keyName = glfwGetKeyName(key, scancode);
    LOG_INFO("+key {}", keyName != nullptr ? keyName : std::to_string(key));
    return;
  }

  auto& inputManager = mRegistry.ctx().get <InputManager> ();

  const int8_t inputDirection = action * 2 - 1;

  inputManager.handleAxisInput(
    InputHwCode(key),
    1.0f, inputDirection,
    mRegistry );
}

void
CasqadiumEngine::textEvent(
  GLFWwindow*,
  uint32_t codepoint )
{
  const auto& io = ImGui::GetIO();

  if ( io.WantCaptureKeyboard == true ||
       io.WantTextInput == true )
    return;

//  todo: text controller ?
}

void
CasqadiumEngine::mouseButtonEvent(
  GLFWwindow*,
  int32_t button,
  int32_t action,
  int32_t mods )
{
  auto& inputManager = mRegistry.ctx().get <InputManager> ();

  const auto inputDirection = action * 2 - 1;

  inputManager.handleAxisInput(
    InputHwCode(MouseInputId::ENUM_START) + button,
    1.0f, inputDirection,
    mRegistry );
}

void
CasqadiumEngine::mousePosEvent(
  GLFWwindow* window,
  double xPos,
  double yPos )
{
  auto& inputManager = mRegistry.ctx().get <InputManager> ();

  const auto deltaX = xPos - mMousePos.x;
  const auto deltaY = yPos - mMousePos.y;

  mMousePos = {xPos, yPos};

  if ( ImGui::GetIO().WantCaptureMouse == true )
    return;

  if ( deltaX != 0.0 )
  {
    inputManager.handleAxisInput(
      InputHwCode(MouseInputId::MoveX),
      std::abs(deltaX),
      std::clamp(deltaX, -1.0, 1.0),
      mRegistry );
  }

  if ( deltaY != 0.0 )
  {
    inputManager.handleAxisInput(
      InputHwCode(MouseInputId::MoveY),
      std::abs(deltaY),
      std::clamp(deltaY, -1.0, 1.0),
      mRegistry );
  }

  inputManager.handleAxisInput(
    InputHwCode(MouseInputId::PosX),
    xPos,
    std::clamp(xPos, -1.0, 1.0),
    mRegistry );

  inputManager.handleAxisInput(
    InputHwCode(MouseInputId::PosY),
    yPos,
    std::clamp(yPos, -1.0, 1.0),
    mRegistry );
}

void
CasqadiumEngine::mouseEnterEvent(
  GLFWwindow* window,
  int32_t entered )
{
  if ( entered == true )
    mHoveredWindow = window;

  else if ( mHoveredWindow == window )
    mHoveredWindow = nullptr;
}

void
CasqadiumEngine::mouseWheelEvent(
  GLFWwindow*,
  double xoffset,
  double yoffset )
{
  auto& inputManager = mRegistry.ctx().get <InputManager> ();

  if ( xoffset != 0.0 )
    inputManager.handleAxisInput(
      InputHwCode(MouseInputId::WheelX),
      std::abs(xoffset),
      std::clamp(xoffset, -1.0, 1.0),
      mRegistry );

  if ( yoffset != 0.0 )
    inputManager.handleAxisInput(
      InputHwCode(MouseInputId::WheelY),
      std::abs(yoffset),
      std::clamp(yoffset, -1.0, 1.0),
      mRegistry );
}

void
CasqadiumEngine::setupEventCallbacks()
{
//  Window

  glfwSetWindowFocusCallback(mWindow,
  [] ( GLFWwindow* window, int focused )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->windowFocusEvent(
      window, focused );
  });

  glfwSetFramebufferSizeCallback(mWindow,
  [] ( GLFWwindow* window, int width, int height )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->framebufferSizeEvent(
      window, width, height );
  });

  glfwSetWindowSizeCallback(mWindow,
  [] ( GLFWwindow* window, int width, int height )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->windowSizeEvent(
      window, width, height );
  });

  glfwSetDropCallback(mWindow,
  [] ( GLFWwindow* window, int pathCount, const char* paths[] )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->dragDropEvent(
      window, pathCount, paths );
  });


//  Input

  glfwSetKeyCallback(mWindow,
  [] ( GLFWwindow* window, int key, int scancode, int action, int mods )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->keyEvent(
      window, key, scancode, action, mods);
  });

  glfwSetCharCallback(mWindow,
  [] ( GLFWwindow* window, unsigned int codepoint )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->textEvent(
      window, codepoint );
  });

  glfwSetMouseButtonCallback(mWindow,
  [] ( GLFWwindow* window, int button, int action, int mods )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->mouseButtonEvent(
      window, button, action, mods );
  });

  glfwSetCursorPosCallback(mWindow,
  [] ( GLFWwindow* window, double xpos, double ypos )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->mousePosEvent(
      window, xpos, ypos );
  });

  glfwSetCursorEnterCallback(mWindow,
  [] ( GLFWwindow* window, int entered )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->mouseEnterEvent(
      window, entered );
  });

  glfwSetScrollCallback(mWindow,
  [] ( GLFWwindow* window, double xoffset, double yoffset )
  {
    auto pointer = glfwGetWindowUserPointer(window);
    if ( pointer == nullptr )
      pointer = ImGui::GetIO().UserData;

    const auto engine = static_cast <CasqadiumEngine*> (pointer);

    engine->mouseWheelEvent(
      window, xoffset, yoffset );
  });
}

} // namespace cqde::types
