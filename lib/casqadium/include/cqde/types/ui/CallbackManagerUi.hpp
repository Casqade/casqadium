#pragma once

#include <cqde/types/ui/widgets/StringFilter.hpp>

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

  StringFilter mCallbackIdFilter {"Callback ID"};

public:
  CallbackManagerUi( types::CallbackManager* );

  void ui_show( entt::registry& );
};

} // namespace cqde::ui
