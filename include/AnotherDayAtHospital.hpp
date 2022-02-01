#ifndef ANOTHER_DAY_AT_HOSPITAL_HPP
#define ANOTHER_DAY_AT_HOSPITAL_HPP

#include <TimeUtils/Duration.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>
#include <olcPGE/olcEventHandler.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <GameStateController.hpp>


class AnotherDayAtHospital : public olc::PixelGameEngine
{
  TimeUtils::Duration mTickInterval;
  TimeUtils::Duration mFrameInterval;

  TimeUtils::Duration mTickPrevious;
  TimeUtils::Duration mFramePrevious;

  GameStateController mGameStateController;
  olc::EventHandler   mEventHandler;
  olc::imgui::PGE_ImGUI mImGui;

  bool loadResources();

  bool update( const uint32_t,
               const TimeUtils::Duration );

public:
  AnotherDayAtHospital() = delete;
  AnotherDayAtHospital( const uint64_t tickRate,
                        const uint64_t frameRate );

  bool OnUserCreate() override;
  bool OnUserUpdate( float ) override;
};

#endif
