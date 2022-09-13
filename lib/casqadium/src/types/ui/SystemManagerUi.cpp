#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/common.hpp>
#include <cqde/callbacks.hpp>

#include <imgui.h>


namespace cqde::ui
{

SystemManagerUi::SystemManagerUi(
  types::SystemManager* systemMgr )
  : mSystemMgr{systemMgr}
{}

void
SystemManagerUi::init(
  const entt::registry& registry )
{
  CQDE_ASSERT_DEBUG(mSystemMgr != nullptr, return);

  mSystemsStateBackup = mSystemMgr->serialize();
  mSystemsStateCurrent = mSystemsStateBackup;
}

void
SystemManagerUi::ui_show(
  entt::registry& registry )
{
  CQDE_ASSERT_DEBUG(mSystemMgr != nullptr, return);

  const auto flags = ImGuiWindowFlags_MenuBar |
                     ImGuiWindowFlags_HorizontalScrollbar;

  if ( ImGui::Begin("Systems", NULL, flags) == false )
  {
    ImGui::End(); // Systems
    return;
  }

  if ( ImGui::BeginMenuBar() )
  {
    using Phase = types::System::Phase;

    if ( ImGui::MenuItem("Apply") )
    {
      mSystemMgr->deserialize(mSystemsStateCurrent);

      for ( const auto& systemId : mSystemMgr->systems(Phase::Editor) )
        mSystemMgr->activate(systemId);
    }

    if ( ImGui::MenuItem("Apply & Run") )
    {
      mSystemMgr->deserialize(mSystemsStateCurrent);
      callbacks::editorModeDisable(registry);
    }

    if ( ImGui::MenuItem("Reset") )
    {
      mSystemsStateCurrent = mSystemsStateBackup;
      mSystemMgr->deserialize(mSystemsStateCurrent);

      for ( const auto& systemId : mSystemMgr->systems(Phase::Editor) )
        mSystemMgr->activate(systemId);
    }

    ImGui::EndMenuBar();
  }

  using Phase = types::System::Phase;

  const auto systemsShow =
  [this] ( const Phase phase )
  {
    for ( auto& system : mSystemMgr->mSystems )
    {
      const auto systemIter =
      [this] ( const SystemId& system )
      {
        for ( auto iter = mSystemsStateCurrent.begin();
              iter != mSystemsStateCurrent.end();
              ++iter )
          if ( iter->asString() == system.str() )
            return iter;

        return mSystemsStateCurrent.end();

      } (system.id);

      bool systemActive = systemIter != mSystemsStateCurrent.end();

      if ( system.phase == phase &&
           ImGui::Checkbox(system.id.str().c_str(), &systemActive) )
      {
        if ( systemActive == true )
          mSystemsStateCurrent.append(system.id.str());
        else
          mSystemsStateCurrent.removeIndex(systemIter.index(), nullptr);
      }
    }
  };

  if ( ImGui::CollapsingHeader("Logic", ImGuiTreeNodeFlags_DefaultOpen) )
    systemsShow(Phase::Logic);

  if ( ImGui::CollapsingHeader("Render", ImGuiTreeNodeFlags_DefaultOpen) )
    systemsShow(Phase::Render);

  ImGui::End(); //  Systems
}

} // namespace cqde::types
