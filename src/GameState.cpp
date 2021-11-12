#include <GameState.hpp>
#include <Logger.hpp>


extern Logger Log;

void
GameState::enter()
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}

void
GameState::render( sf::RenderWindow& )
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}

void
GameState::update( const sf::Time )
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}

void
GameState::handleEvent( const sf::Event event )
{
  switch (event.type)
  {
    case sf::Event::EventType::KeyPressed:
      keyEvent( event.key );
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
GameState::keyEvent( const sf::Event::KeyEvent )
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}

void
GameState::mouseMoveEvent( const sf::Event::MouseMoveEvent )
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}

void
GameState::mouseButtonEvent( const sf::Event::MouseButtonEvent )
{
  Log << __FUNCTION__ + ":"_s + std::to_string(__LINE__);
}
