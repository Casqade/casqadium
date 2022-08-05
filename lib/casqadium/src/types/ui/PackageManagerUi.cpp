#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::ui
{

PackageManagerUi::PackageManagerUi(
  types::PackageManager* packageMgr )
  : mPackageMgr{packageMgr}
{}

void
PackageManagerUi::ui_show(
  entt::registry& registry )
{
  CQDE_ASSERT_DEBUG(mPackageMgr != nullptr, return);

  if ( ImGui::Begin("Package management", nullptr,
                    ImGuiWindowFlags_HorizontalScrollbar ) == false )
  {
    ImGui::End(); // Package management
    return;
  }



  ImGui::End(); // Package management
}

} // namespace cqde::ui
