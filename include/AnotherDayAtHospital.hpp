#pragma once

#include <TimeUtils/Duration.hpp>

#include <cqde/types/ConfigManager.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>
#include <olcPGE/olcEventHandler.hpp>
#include <olcPGE/olcPGEX_ImGui.hpp>

#include <GameStateController.hpp>


class AnotherDayAtHospital : public olc::PixelGameEngine
{
  using ConfigManager = cqde::types::ConfigManager;

  ConfigManager mConfigManager {};

  TimeUtils::Duration mTickInterval {};
  TimeUtils::Duration mFrameInterval {};

  TimeUtils::Duration mTickPrevious {TimeUtils::Now()};
  TimeUtils::Duration mFramePrevious {mTickPrevious};

  GameStateController mGameStateController {};
  olc::EventHandler   mEventHandler {this};
  olc::imgui::PGE_ImGUI mImGui {true};

  uint8_t mGameLayer {};

  bool update( const uint32_t,
               const TimeUtils::Duration );

public:
  AnotherDayAtHospital() = delete;
  ~AnotherDayAtHospital();
  AnotherDayAtHospital( const ConfigManager& );

  bool OnUserCreate() override;
  bool OnUserUpdate( float ) override;
};
