#ifndef GAME_STATE_FOREWORD_HPP
#define GAME_STATE_FOREWORD_HPP

#include <GameState.hpp>
#include <Widgets.hpp>

#include <set>


class GameStateForeword
  : public GameState
{
  enum class StateLocal
  {
    TransitingIn,
    WaitAccept,
    TransitingOut,

  } mState;

//  sf::Text mForeword;
  InputPrompt mInputPrompt;

  FadeEffect mFade;

  TimeUtils::Duration mInputPromptDelay;
  TimeUtils::Duration mTransitingOutTimer;

  std::set <olc::Key> mPressedKeys;

  void transitingIn( const TimeUtils::Duration );
  void transitingOut( const TimeUtils::Duration );

  void waitAccept( const TimeUtils::Duration );

  void keyEvent( const olc::Event ) override;

public:
  GameStateForeword( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render( olc::PixelGameEngine* const ) override;
};


#endif
