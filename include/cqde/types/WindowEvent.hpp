#pragma once

#include <cstdint>


namespace cqde::types
{

class WindowEvent
{
public:
  struct KeyEvent
  {
    int32_t code;
  };

  struct TextEvent
  {
    uint32_t codepoint;
  };

  struct MouseMoveEvent
  {
    double x, dx;
    double y, dy;
  };

  enum class MouseButton
  {
    Left,
    Right,
    Middle,
    XButton1,
    XButton2,
    ButtonCount,
  };

  struct MouseButtonEvent
  {
    MouseButton button;
    double x;
    double y;
  };

  struct MouseWheelScrollEvent
  {
    double dx;
    double dy;
  };

  struct WindowResizeEvent
  {
    vi2d newSize;
    vi2d oldSize;
  };

  enum EventType
  {
    TextEntered,

    KeyReleased,
    KeyHeld,
    KeyPressed,

    MouseButtonReleased,
    MouseButtonHeld,
    MouseButtonPressed,

    MouseWheelScrolled,
    MouseMoved,

    MouseEntered,
    MouseLeft,

    FramebufferResized,

    WindowMoved,
    WindowResized,
    WindowClosed,
    WindowRefreshed,
    WindowFocused,
    WindowIconified,
    WindowMaximized,
    WindowContentScaled,

    Count
  };

  EventType type;

  union
  {
    KeyEvent              key;
    TextEvent             text;
    MouseMoveEvent        mouseMove;
    MouseButtonEvent      mouseButton;
    MouseWheelScrollEvent mouseWheelScroll;
    WindowResizeEvent     windowResize {};
  };
};

} // namespace cqde::types
