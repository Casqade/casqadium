#pragma once

#include <cqde/fwd.hpp>
#include <cqde/alias.hpp>

#include <cqde/types/ui/widgets/PackageFilter.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <cqde/types/UndoRedoQueue_types.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::ui
{

class PrefabManagerUi
{
  using PrefabsState = std::map <PackageId, Json::Value>;
  using UndoRedoQueue = types::UndoRedoQueue <PrefabsState>;

  PackageFilter mPackageFilter {};
  StringFilter mPrefabFilter {"Prefab ID"};

  PrefabsState mPrefabsState {};

  UndoRedoQueue mHistoryBuffer {32};

  std::string mNewPrefabId {"NewPrefab"};

  Json::Value mClipboard {};

  PrefabId mSelectedPrefabId {};
  std::string mRenamedPrefabId {};

  types::PrefabManager* mPrefabMgr {};

public:
  PrefabManagerUi( types::PrefabManager* );

  void ui_show( entt::registry& );

private:
  void prefabsApply( const entt::registry& );
  void prefabsSave( const PackageId&, entt::registry& );
  void ui_show_menu_bar( entt::registry& );
};

} // namespace cqde::ui
