#include <olcPGE/olcEventHandler.hpp>


namespace olc
{

EventHandler::EventHandler( const PixelGameEngine* const pge )
  : mPGE(pge)
{}

void
EventHandler::updateEvents()
{
  const bool isInFocus = mPGE->IsFocused();
  static bool wasInFocus = isInFocus;

  if ( isInFocus != wasInFocus )
  {
    Event event;

    if ( isInFocus )
      event.type = Event::EventType::GainedFocus;
    else
      event.type = Event::EventType::LostFocus;

    mEvents.push_back(event);
  }

  if ( isInFocus == false )
    return;

  for ( int32_t key = 0;
        key < Key::ENUM_END;
        ++key )
  {
    Event event;

    const HWButton keyState = mPGE->GetKey(Key(key));

    if ( keyState.bPressed )
      event.type = Event::EventType::KeyPressed;

    else if ( keyState.bReleased )
      event.type = Event::EventType::KeyReleased;

    else
      continue;

    event.key.code = key;
    mEvents.push_back(event);
  }

  const vi2d mousePos = mPGE->GetMousePos();
  static vi2d mousePosPrev = mousePos;

  if ( mousePos != mousePosPrev )
  {
    Event event;

    event.type = Event::EventType::MouseMoved;

    event.mouseMove.x = mousePos.x;
    event.mouseMove.y = mousePos.y;
    event.mouseMove.dx = mousePos.x - mousePosPrev.x;
    event.mouseMove.dy = mousePos.y - mousePosPrev.y;

    mEvents.push_back(event);
  }

  for ( uint32_t button = 0;
        button < olc::nMouseButtons;
        ++button )
  {
    Event event;

    const HWButton mouseState = mPGE->GetMouse(button);

    if ( mouseState.bPressed )
      event.type = Event::EventType::MouseButtonPressed;

    else if ( mouseState.bReleased )
      event.type = Event::EventType::MouseButtonReleased;

    else
      continue;

    event.mouseButton.button = Event::MouseButton(button);
    event.mouseButton.x = mousePos.x;
    event.mouseButton.y = mousePos.y;

    mEvents.push_back(event);
  }

  const int32_t mouseWheel = mPGE->GetMouseWheel();
  static int32_t mouseWheelPrev = mouseWheel;

  if ( mouseWheel != mouseWheelPrev )
  {
    Event event;

    event.type = Event::EventType::MouseWheelScrolled;

    event.mouseWheelScroll.delta = mouseWheel;
    event.mouseWheelScroll.x = mousePos.x;
    event.mouseWheelScroll.y = mousePos.y;

    mEvents.push_back(event);
  }

  return;
}

bool
EventHandler::pollEvent( Event& event )
{
  updateEvents();

  if ( mEvents.empty() )
    return false;

  event = mEvents.front();
  mEvents.pop_front();

  return true;
}

} // namespace olc