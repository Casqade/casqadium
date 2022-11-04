#include <olcPGE/olcEventHandler.hpp>

#include <olcPGE/olcPGEX_ImGui.hpp>


namespace olc
{

EventHandler::EventHandler( const PixelGameEngine* const pge )
  : mPGE(pge)
{}

bool
EventHandler::acceptKeyHeldEvent(
  const Event& event ) const
{
  using EventType = Event::EventType;

  if ( event.type != EventType::KeyHeld )
    return true;

  for ( auto iter = mEvents.rbegin();
        iter < mEvents.rend();
        ++iter )
  {
    if (  event.key.code != iter->key.code ||
          iter->type != EventType::KeyHeld )
      continue;

    for ( ; iter > mEvents.rbegin();
          --iter )
      if (  event.key.code == iter->key.code &&
            iter->type == EventType::KeyPressed )
        return true;

    return false;
  }

  return true;
}

bool
EventHandler::acceptButtonHeldEvent(
  const Event& event ) const
{
  using EventType = Event::EventType;

  if ( event.type != EventType::MouseButtonHeld )
    return true;

  for ( auto iter = mEvents.rbegin();
        iter < mEvents.rend();
        ++iter )
  {
    if (  event.mouseButton.button != iter->mouseButton.button ||
          iter->type != EventType::MouseButtonHeld )
      continue;

    for ( ; iter > mEvents.rbegin();
          --iter )
      if (  event.mouseButton.button == iter->mouseButton.button &&
            iter->type == EventType::MouseButtonPressed )
        return true;

    return false;
  }

  return true;
}

void
EventHandler::update()
{
  using EventType = Event::EventType;

  const bool isInFocus = mPGE->IsFocused();
  static bool wasInFocus = isInFocus;

  if ( isInFocus != wasInFocus )
  {
    Event event;

    if ( isInFocus )
      event.type = EventType::GainedFocus;
    else
      event.type = EventType::LostFocus;

    mEvents.push_back(event);

    wasInFocus = isInFocus;
  }

  if ( isInFocus == false )
    return;

  const vi2d windowSize = mPGE->GetWindowSize();
  static vi2d windowSizePrev = windowSize;

  if ( windowSize != windowSizePrev )
  {
    Event event;

    event.type = EventType::WindowResized;

    event.windowResize.newSize = windowSize;
    event.windowResize.oldSize = windowSizePrev;

    mEvents.push_back(event);

    windowSizePrev = windowSize;
  }

  for ( int32_t key = 0;
        key < Key::ENUM_END;
        ++key )
  {
    if ( ImGui::GetIO().WantCaptureKeyboard == true )
      continue;

    Event event;

    const HWButton keyState = mPGE->GetKey(Key(key));

    if ( keyState.bPressed )
      event.type = EventType::KeyPressed;

    else if ( keyState.bReleased )
      event.type = EventType::KeyReleased;

    else if ( keyState.bHeld )
      event.type = EventType::KeyHeld;

    else
      continue;

    event.key.code = Key(key);

    if ( acceptKeyHeldEvent(event) == true )
      mEvents.push_back(event);
  }

  const vi2d mousePos = mPGE->GetMousePos();
  static vi2d mousePosPrev = mousePos;

  if ( mousePos != mousePosPrev )
  {
    if ( mPGE->GetKeepMouseCentered() == true )
    {
      mousePosPrev.x = mPGE->GetWindowSize().x / 2;
      mousePosPrev.y = mPGE->GetWindowSize().y / 2;
    }

    Event event;

    event.type = EventType::MouseMoved;

    event.mouseMove.x = mousePos.x;
    event.mouseMove.y = mousePos.y;
    event.mouseMove.dx = mousePos.x - mousePosPrev.x;
    event.mouseMove.dy = mousePos.y - mousePosPrev.y;

    mEvents.push_back(event);

    mousePosPrev = mousePos;
  }

  for ( uint32_t button = 0;
        button < olc::nMouseButtons;
        ++button )
  {
    Event event;

    const HWButton mouseState = mPGE->GetMouse(button);

    if ( mouseState.bPressed )
      event.type = EventType::MouseButtonPressed;

    else if ( mouseState.bReleased )
      event.type = EventType::MouseButtonReleased;

    else if ( mouseState.bHeld )
      event.type = EventType::MouseButtonHeld;

    else
      continue;

    event.mouseButton.button = Event::MouseButton(button);

    if ( acceptButtonHeldEvent(event) == true )
    {
      event.mouseButton.x = mousePos.x;
      event.mouseButton.y = mousePos.y;

      mEvents.push_back(event);
    }
  }

  const int32_t mouseWheel = mPGE->GetMouseWheel();
  static int32_t mouseWheelPrev = mouseWheel;

  if ( mouseWheel != mouseWheelPrev )
  {
    Event event;

    event.type = EventType::MouseWheelScrolled;

    event.mouseWheelScroll.delta = mouseWheel;
    event.mouseWheelScroll.x = mousePos.x;
    event.mouseWheelScroll.y = mousePos.y;

    mEvents.push_back(event);

    mouseWheelPrev = mouseWheel;
  }

  return;
}

bool
EventHandler::pollEvent( Event& event )
{
  if ( mEvents.empty() )
    return false;

  event = mEvents.front();
  mEvents.pop_front();

  return true;
}

} // namespace olc
