#ifndef GAME_STATE_FOREWORD_HPP
#define GAME_STATE_FOREWORD_HPP

#include <GameState.hpp>
#include <Widgets.hpp>

#include <set>


namespace sf
{
class RenderWindow;
}


class GameStateForeword
  : public GameState
{
  enum class StateLocal
  {
    TransitingIn,
    WaitAccept,
    TransitingOut,

  } mState;

  sf::Text mForeword;
  InputPrompt mInputPrompt;

  FadeEffect mFade;

  float mInputPromptDelay;
  float mTransitingOutTimer;

  std::set <SDL_Scancode> mPressedKeys;

  void transitingIn( const float dt );
  void transitingOut( const float dt );

  void waitAccept( const float dt );

  void keyEvent( const SDL_Event ) override;

public:
  GameStateForeword();
  void update( const TimeUtils::Duration ) override;
  void render( sf::RenderWindow& ) override;
};


#endif
