#pragma once

#include <entt/fwd.hpp>


namespace cqde::types
{
class SystemManager;
}


namespace cqde::ui
{

class SystemManagerUi
{
  types::SystemManager* mSystemMgr {};

public:
  SystemManagerUi( types::SystemManager* );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
