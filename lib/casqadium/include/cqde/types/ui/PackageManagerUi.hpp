#pragma once

#include <entt/fwd.hpp>


namespace cqde::types
{
class PackageManager;
}

namespace cqde::ui
{

class PackageManagerUi
{
  types::PackageManager* mPackageMgr {};

public:
  PackageManagerUi( types::PackageManager* );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
