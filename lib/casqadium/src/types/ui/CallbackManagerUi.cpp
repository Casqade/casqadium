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

  if ( ImGui::Begin("Callbacks", nullptr,
                    ImGuiWindowFlags_HorizontalScrollbar ) == false )
  {
    ImGui::End(); // Callbacks
    return;
  }

  for ( const auto& [callbackId, callback] : mCallbackMgr->mCallbacks )
    ImGui::Text("%s", callbackId.str().c_str());

  ImGui::End(); // Callbacks
}

} // namespace cqde::ui
