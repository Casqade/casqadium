#include <GameState.hpp>

#include <cqde/util/logger.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>

#include <cxxabi.h>


template <typename Type>
std::string getTypeName( const Type* type)
{
  std::string typeName = typeid( typename std::remove_pointer <Type>::type ).name();
  char* demangledName = abi::__cxa_demangle(typeName.c_str(), NULL, NULL, NULL);
  if ( demangledName != nullptr )
  {
    typeName = demangledName;
    std::free(demangledName);
  }

  return typeName;
}

GameState::GameState(
  GameStateController* const stateController )
  : mGameStateController(stateController)
  , mPGE(olc::renderer->ptrPGE)
{}

GameState::~GameState()
{
  LOG_TRACE("Destroying state {}", getTypeName ( this ));
}

bool
GameState::update(
  const uint32_t ticks,
  const TimeUtils::Duration& )
{
  LOG_TRACE("{}", __FUNCTION__);

  return true;
}

void
GameState::render(
  const uint32_t frames,
  const TimeUtils::Duration& )
{
  LOG_TRACE("{}", __FUNCTION__);
}

void
GameState::handleEvent( const olc::Event event )
{
  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
    case olc::Event::EventType::KeyReleased:
      keyEvent( event );
      break;

    case olc::Event::EventType::MouseMoved:
      mouseMoveEvent( event.mouseMove );
      break;

    case olc::Event::EventType::MouseButtonPressed:
    case olc::Event::EventType::MouseButtonReleased:
      mouseButtonEvent( event );
      break;

    case EventType::WindowResized:
      windowResizeEvent(event.windowResize);
      break;

    default:
      break;
  }
}

void
GameState::keyEvent( const olc::Event )
{
  LOG_TRACE("{}", __FUNCTION__);
}

void
GameState::mouseMoveEvent( const olc::Event::MouseMoveEvent )
{
  LOG_TRACE("{}", __FUNCTION__);
}

void
GameState::mouseButtonEvent( const olc::Event )
{
  LOG_TRACE("{}", __FUNCTION__);
}

void
GameState::windowResizeEvent( const olc::Event::WindowResizeEvent )
{
  LOG_TRACE("{}", __FUNCTION__);
}
