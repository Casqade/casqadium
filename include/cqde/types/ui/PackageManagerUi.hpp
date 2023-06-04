#pragma once

#include <cqde/fwd.hpp>
#include <cqde/alias.hpp>

#include <cqde/types/UndoRedoQueue_types.hpp>

#include <entt/fwd.hpp>

#include <json/value.h>


namespace cqde::ui
{

class PackageManagerUi
{
  struct ConfigState
  {
    Json::Value root {Json::objectValue};
    Json::Value packages {Json::objectValue};

    bool operator == ( const ConfigState& ) const;

  } mConfigState {};

  using UndoRedoQueue = types::UndoRedoQueue <ConfigState>;

  UndoRedoQueue mHistoryBuffer {32};

  std::string mPackageNewName {"NewPackage"};
  std::string mEditedPackageId {};

  Json::ArrayIndex mDraggedPackageIndex {-1u};

  bool mPackageWindowOpened {};
  bool mPackageReloadRequested {};

  types::PackageManager* mPackageMgr {};

public:
  PackageManagerUi( types::PackageManager* );

  void ui_show( entt::registry& );

private:
  void ui_show_menu_bar( entt::registry& );
  void ui_show_package_window( entt::registry& );
};

} // namespace cqde::ui
