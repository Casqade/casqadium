#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/PackageFilter.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>

#include <map>
#include <deque>


namespace cqde::types
{
class InputManager;
}

namespace cqde::ui
{

class InputManagerUi
{
  using InputConfigs = std::map <PackageId, Json::Value>;

  ui::PackageFilter mPackageFilter {};

  InputConfigs mInputConfigs {};

  size_t mHistoryBufferSize {32};
  std::deque <InputConfigs> mHistoryBuffer {};
  std::deque <InputConfigs>::iterator mHistoryBufferIter {};

  Json::Value mClipboardAxis {};
  Json::Value mClipboardBinding {};

  std::string mNewAxisName {"NewAxis"};
  std::string mRenamedAxisId {};

  InputAxisId mSelectedAxis {};
  InputHwId mSelectedBinding {};

  bool mConfigChanged {};
  bool mBindingWindowOpened {};

  types::InputManager* mInputMgr {};


public:
  InputManagerUi( types::InputManager* );

  void ui_show( entt::registry& );


private:
  void configApply( entt::registry& );
  void configLoad( const PackageId&, entt::registry& );
  void configSave( const PackageId&, entt::registry& );

  void configHistoryPush();
  void undo();
  void redo();

  bool undoAvailable() const;
  bool redoAvailable() const;

  void ui_show_menu_bar( entt::registry& );
  void ui_show_binding_window();
};

} // namespace cqde::ui
