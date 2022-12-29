#pragma once

#include <CasqadiumState.hpp>

#include <cqde/alias.hpp>

#include <entt/entt.hpp>


class CasqadiumStateDemo
  : public CasqadiumState
{
  using ConfigManager = cqde::types::ConfigManager;

  entt::registry mRegistry {};

  bool mRunning {true};

  void keyEvent( const olc::Event ) override;
  void mouseMoveEvent( const olc::Event::MouseMoveEvent ) override;
  void mouseButtonEvent( const olc::Event ) override;
  void mouseWheelEvent( const olc::Event::MouseWheelScrollEvent ) override;

public:
  CasqadiumStateDemo( const ConfigManager& );
  ~CasqadiumStateDemo();

  bool update( const uint32_t ticks,
               const TimeUtils::Duration& interval ) override;
  void render( const uint32_t frames,
               const TimeUtils::Duration& interval ) override;
};
