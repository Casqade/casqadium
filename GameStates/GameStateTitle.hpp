#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <GameState.hpp>

#include <set>


struct SDL_Scancode;
struct SDL_Event;

namespace Math
{
class Vector3f;
}

namespace TimeUtils
{
class Duration;
}

class GameStateTitle
  : public GameState
{
  enum class StateLocal
  {
    Title,
    Backstory,

  } mState;

  Math::Vector3f mTitlePos;
  Math::Vector3f mBackstoryPos;

  sf::VertexArray mLines;

  std::set <SDL_Scancode> mPressedKeys;

  void handleControls( const float dt );

  void updateLines( const float dt );
  void updateTitle( const float dt );
  void updateBackStory( const float dt );

  void keyEvent( const SDL_Event ) override;

public:
  GameStateTitle();
//  void enter() override;
  void update( const TimeUtils::Duration ) override;
  void render( sf::RenderWindow& ) override;
};


#endif
