#ifndef GAME_STATE_TITLE_HPP
#define GAME_STATE_TITLE_HPP

#include <GameState.hpp>

#include <set>
#include <glm/vec3.hpp>


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

  glm::vec3 mTitlePos;
  glm::vec3 mBackstoryPos;

  std::set <olc::Key> mPressedKeys;

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
