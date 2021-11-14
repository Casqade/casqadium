#include <GameState.hpp>
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
GameState::render( sf::RenderWindow& )
{
  Log << __FUNCTION__;
}

void
GameState::update( const sf::Time )
{
  Log << __FUNCTION__;
}

void
GameState::handleEvent( const sf::Event event )
{
  switch (event.type)
  {
    case sf::Event::EventType::KeyPressed:
    case sf::Event::EventType::KeyReleased:
      keyEvent( event );
      break;

    case sf::Event::EventType::MouseMoved:
      mouseMoveEvent( event.mouseMove );
      break;

    case sf::Event::EventType::MouseButtonPressed:
      mouseButtonEvent( event.mouseButton );
      break;

    default:
      break;
  }
}

void
GameState::keyEvent( const sf::Event )
{
  Log << __FUNCTION__;
}

void
GameState::mouseMoveEvent( const sf::Event::MouseMoveEvent )
{
  Log << __FUNCTION__;
}

void
GameState::mouseButtonEvent( const sf::Event::MouseButtonEvent )
{
  Log << __FUNCTION__;
}
