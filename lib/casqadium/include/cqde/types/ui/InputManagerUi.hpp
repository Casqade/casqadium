#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/PackageFilter.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/UndoRedoQueue.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>

#include <map>


namespace cqde::types
{
class InputManager;

extern template class
UndoRedoQueue <std::map <PackageId, Json::Value>>;
}

namespace cqde::ui
{

class InputManagerUi
{
  using InputConfigs = std::map <PackageId, Json::Value>;
  using UndoRedoQueue = types::UndoRedoQueue <InputConfigs>;

  ui::PackageFilter mPackageFilter {};
  ui::StringFilter mAxisFilter {"Axis ID"};

  InputConfigs mInputConfigs {};

  UndoRedoQueue mHistoryBuffer {32};

  Json::Value mClipboardAxis {};
  Json::Value mClipboardBinding {};

  std::string mNewAxisName {"NewAxis"};
  std::string mRenamedAxisId {};

  InputAxisId mSelectedAxis {};
  InputHwId mSelectedBinding {};

  bool mBindingWindowOpened {};

  types::InputManager* mInputMgr {};

public:
  InputManagerUi( types::InputManager* );

  void ui_show( entt::registry& );


private:
  void configApply( entt::registry& );
  void configSave( const PackageId&, entt::registry& );

  void ui_show_menu_bar( entt::registry& );
  void ui_show_binding_window();
};

} // namespace cqde::ui
