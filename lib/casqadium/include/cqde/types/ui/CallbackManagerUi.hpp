#pragma once

#include <entt/fwd.hpp>


namespace cqde::types
{
class CallbackManager;
}

namespace cqde::ui
{

class CallbackManagerUi
{
  types::CallbackManager* mCallbackMgr {};

public:
  CallbackManagerUi( types::CallbackManager* );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
