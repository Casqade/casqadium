#ifndef OLC_PGE_EVENT
#define OLC_PGE_EVENT

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
  };
};

} // namespace olc

#endif
