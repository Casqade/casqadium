#pragma once

#include <olcPGE/olcPixelGameEngine.hpp>


namespace olc
{

class Event
{
public:
  struct KeyEvent
  {
    olc::Key code;
  };

  struct TextEvent
  {
    char unicode;
  };

  struct MouseMoveEvent
  {
    int x, dx;
    int y, dy;
  };

  enum class MouseButton
  {
    Left,
    Right,
    Middle,
    XButton1,
    XButton2,
    ButtonCount
  };

  struct MouseButtonEvent
  {
    MouseButton button;
    int         x;
    int         y;
  };

  struct MouseWheelScrollEvent
  {
    int delta;
    int x;
    int y;
  };

  struct WindowResizeEvent
  {
    vi2d newSize;
    vi2d oldSize;
  };

  enum EventType
  {
    LostFocus,
    GainedFocus,
    TextEntered,
    KeyPressed,
    KeyReleased,
    MouseWheelScrolled,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseEntered,
    MouseLeft,
    WindowResized,

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

} // namespace olc
