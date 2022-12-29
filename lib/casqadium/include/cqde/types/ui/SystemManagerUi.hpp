#pragma once

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::types
{
class SystemManager;
}


namespace cqde::ui
{

class SystemManagerUi
{
  StringFilter mSystemFilter {"System ID"};

  Json::Value mSystemsStateInitial {Json::arrayValue};
  Json::Value mSystemsStateCurrent {Json::arrayValue};

  bool mRunning {};
  bool mPauseRequested {};

  types::SystemManager* mSystemMgr {};

public:
  SystemManagerUi( types::SystemManager* );

  void init( const entt::registry& );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
