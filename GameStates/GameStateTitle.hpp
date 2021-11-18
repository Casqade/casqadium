#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <GameState.hpp>
#include <SFML/Graphics/Text.hpp>

#include <set>


namespace sf
{
class Time;
class RenderWindow;
}

class GameStateTitle
  : public GameState
{
  enum class StateLocal
  {
    Title,
    Backstory,

  } mState;

  sf::Vector3f mTitlePos;
  sf::Vector3f mBackstoryPos;

  sf::VertexArray mLines;

  std::set <sf::Keyboard::Key> mPressedKeys;

  void handleControls( const float dt );

  void updateLines( const float dt );
  void updateTitle( const float dt );
  void updateBackStory( const float dt );

  void keyEvent( const sf::Event ) override;

public:
  GameStateTitle();
//  void enter() override;
  void update( const sf::Time ) override;
  void render( sf::RenderWindow& ) override;
};


#endif
