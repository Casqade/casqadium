#include <GameStateTitle.hpp>
#include <Variables.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>


extern std::vector <sf::Text> IntroTextEntries;

void
GameStateTitle::update( const sf::Time elapsed )
{
  const float dt = elapsed.asSeconds();
  const float textSpeedY = 20.0f * dt;
  const float textScaleD = 1.0f - dt;

  const float textTargetY = window.getSize().y * 0.5f;

  for ( auto& text : IntroTextEntries )
  {
    if ( text.getPosition().y > window.getSize().y * 0.5f )
      text.move( 0.0f, -textSpeedY );

    if ( text.getScale().x > 0.0f )
      text.scale( textScaleD, textScaleD * 1.0025f );
  }
}

void
GameStateTitle::render( sf::RenderWindow& window )
{
  for ( auto& text : IntroTextEntries )
    window.draw( text );
}
