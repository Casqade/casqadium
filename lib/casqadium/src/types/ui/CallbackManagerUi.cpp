#include <cqde/types/ui/CallbackManagerUi.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <cqde/common.hpp>

#include <imgui.h>


namespace cqde::ui
{

CallbackManagerUi::CallbackManagerUi(
  types::CallbackManager* callbackMgr )
  : mCallbackMgr{callbackMgr}
{}

void
CallbackManagerUi::ui_show(
  entt::registry& registry )
{
  CQDE_ASSERT_DEBUG(mCallbackMgr != nullptr, return);

  if ( ImGui::Begin("Callbacks") == false )
    return ImGui::End(); // Callbacks

  mCallbackIdFilter.search();

  ImGui::Separator();

  const auto flags = ImGuiTableFlags_ScrollX |
                     ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "CallbacksList", 1, flags,
                          ImGui::GetContentRegionAvail()) )
  {
    ImGui::TableNextColumn();

    for ( const auto& [callbackId, callback] : mCallbackMgr->mCallbacks )
      if ( mCallbackIdFilter.query(callbackId.str()) == true )
        ImGui::Text("%s", callbackId.str().c_str());

    ImGui::EndTable(); // CallbacksList
  }

  ImGui::End(); // Callbacks
}

} // namespace cqde::ui
