#pragma once

#include <TimeUtils/Duration.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>
#include <olcPGE/olcEventHandler.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <GameStateController.hpp>


class AnotherDayAtHospital : public olc::PixelGameEngine
{
  TimeUtils::Duration mTickInterval;
  TimeUtils::Duration mFrameInterval;

  TimeUtils::Duration mTickPrevious {TimeUtils::Now()};
  TimeUtils::Duration mFramePrevious {TimeUtils::Now()};

  GameStateController mGameStateController {};
  olc::EventHandler   mEventHandler {this};
  olc::imgui::PGE_ImGUI mImGui {true};

  uint32_t mGameLayer {};

  bool update( const uint32_t,
               const TimeUtils::Duration );

public:
  AnotherDayAtHospital() = delete;
  ~AnotherDayAtHospital();
  AnotherDayAtHospital( const uint64_t tickRate,
                        const uint64_t frameRate );

  bool OnUserCreate() override;
  bool OnUserUpdate( float ) override;
};
