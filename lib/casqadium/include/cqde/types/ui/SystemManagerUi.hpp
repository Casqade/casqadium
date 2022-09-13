#pragma once

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
  Json::Value mSystemsStateBackup {Json::objectValue};
  Json::Value mSystemsStateCurrent {Json::objectValue};

  types::SystemManager* mSystemMgr {};

public:
  SystemManagerUi( types::SystemManager* );

  void init( const entt::registry& );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
