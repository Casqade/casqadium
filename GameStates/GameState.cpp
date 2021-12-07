#include <GameState.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>

#include <Logger.hpp>
#include <Variables.hpp>
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

GameState::GameState( GameStateController* const stateController )
  : mGameStateController(stateController)
  , mPGE(olc::renderer->ptrPGE)
{}

GameState::~GameState()
{
  Log << "Destroying state "_s + getTypeName ( this );
}

void
GameState::enter()
{
  Log << __FUNCTION__;
}

void
GameState::render()
{
  Log << __FUNCTION__;
}

bool
GameState::update( const uint32_t ticks,
                   const TimeUtils::Duration )
{
  Log << __FUNCTION__;

  return true;
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
      mouseButtonEvent( event.mouseButton );
      break;

    default:
      break;
  }
}

void
GameState::keyEvent( const olc::Event )
{
  Log << __FUNCTION__;
}

void
GameState::mouseMoveEvent( const olc::Event::MouseMoveEvent )
{
  Log << __FUNCTION__;
}

void
GameState::mouseButtonEvent( const olc::Event::MouseButtonEvent )
{
  Log << __FUNCTION__;
}
