#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <GameState.hpp>
#include <olcPGE/Math.hpp>

#include <set>


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

  olc::Vector3f mTitlePos;
  olc::Vector3f mBackstoryPos;

  std::vector <olc::vf2d> mLines;

  std::set <olc::Key> mPressedKeys;

  void handleControls( const TimeUtils::Duration );

  void updateLines( const TimeUtils::Duration );
  void updateTitle( const TimeUtils::Duration );
  void updateBackStory( const TimeUtils::Duration );

  void keyEvent( const olc::Event ) override;

public:
  GameStateTitle( GameStateController* const );

  bool update( const uint32_t ticks,
               const TimeUtils::Duration ) override;
  void render() override;
};


#endif
