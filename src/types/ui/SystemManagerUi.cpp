#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/assert.hpp>
#include <cqde/callbacks.hpp>

#include <imgui.h>

#include <algorithm>


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

  mSystemsStateInitial = mSystemMgr->serialize();
  mSystemsStateCurrent = mSystemsStateInitial;

  mRunning = false;
  mPauseRequested = false;
}

void
SystemManagerUi::ui_show(
  entt::registry& registry )
{
  using Phase = types::System::Phase;

  CQDE_ASSERT_DEBUG(mSystemMgr != nullptr, return);

  if ( ImGui::Begin("Systems", NULL,
                    ImGuiWindowFlags_MenuBar) == false )
  {
    ImGui::End(); // Systems
    return;
  }

  const bool wasRunning = mRunning;

  if ( mPauseRequested == true )
  {
    mRunning = false;
    mPauseRequested = false;
  }

  if ( ImGui::BeginMenuBar() )
  {
    if ( ImGui::MenuItem("Apply & Play") )
    {
      mSystemMgr->deserialize(mSystemsStateCurrent);
      callbacks::editorModeDisable(registry);
    }

    if ( ImGui::IsItemHovered() )
      ImGui::SetTooltip("Activate selected systems & exit editor mode");

    if ( ImGui::MenuItem("Reset") )
    {
      mSystemsStateCurrent = mSystemsStateInitial;
      mSystemMgr->deserialize(mSystemsStateCurrent);

      for ( const auto& systemId : mSystemMgr->systems(Phase::Editor) )
        mSystemMgr->activate(systemId);
    }

    if ( ImGui::IsItemHovered() )
      ImGui::SetTooltip("Reset pre-editor systems state");

    ImGui::EndMenuBar();
  }

  if ( mRunning == true )
  {
    if ( ImGui::Button("||##stepPause") )
      mRunning = false;

    if ( ImGui::IsItemHovered() )
      ImGui::SetTooltip("Deactivate selected systems");
  }
  else
  {
    if ( ImGui::Button(">##stepPlay") )
      mRunning = true;

    if ( ImGui::IsItemHovered() )
      ImGui::SetTooltip("Activate selected systems");
  }

  ImGui::SameLine();

  ImGui::Button("|>##stepRepeat");

  if ( ImGui::IsItemActive() )
    mRunning = true;

  if ( ImGui::IsItemDeactivated() )
    mRunning = false;

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Hold for repeated steps");

  ImGui::SameLine();

  if ( ImGui::Button("||>##stepSingle") )
  {
    mRunning = true;
    mPauseRequested = true;
  }

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Step with selected systems");

  if ( mRunning != wasRunning )
  {
    if ( mRunning == true )
    {
      mSystemMgr->deserialize(mSystemsStateCurrent);

      for ( const auto& systemId : mSystemMgr->systems(Phase::Editor) )
        mSystemMgr->activate(systemId);
    }
    else
    {
      mSystemMgr->deactivate();

      for ( const auto& systemId : mSystemMgr->systems(Phase::Editor) )
        mSystemMgr->activate(systemId);
    }
  }

  ImGui::Separator();

  const auto systemsShow =
  [this] ( const Phase phase )
  {
    ImGui::TableNextColumn();

    ImGui::PushID(phase);

    if ( ImGui::SmallButton("+##systemAdd") )
      ImGui::OpenPopup("##systemAddPopup");

    if ( ImGui::BeginPopup("##systemAddPopup") )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      mSystemFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      bool systemsFound {};

      for ( const auto& system : mSystemMgr->mSystems )
      {
        if ( system.phase != phase )
          continue;

        if ( mSystemFilter.query(system.id.str()) == false )
          continue;

        const auto predicate =
        [systemId = system.id.str()] ( const Json::Value& value )
        {
          return value.asString() == systemId;
        };

        if (  std::any_of(mSystemsStateCurrent.begin(),
                          mSystemsStateCurrent.end(),
                          predicate) == true )
          continue;

        systemsFound = true;

        if ( ImGui::Selectable(system.id.str().c_str(), false) )
        {
          mSystemsStateCurrent.append(system.id.str());
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      if ( systemsFound == false )
        ImGui::Text("No systems matching filter");

      ImGui::EndPopup(); // systemAddPopup
    }

    ImGui::Separator();

    const auto tableFlags = ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_ScrollY;

    if ( ImGui::BeginTable("SystemsTable", 1, tableFlags) )
    {
      ImGui::TableNextColumn();

      for ( Json::ArrayIndex i = 0;
            i < mSystemsStateCurrent.size();
            ++i )
      {
        const auto& system = mSystemsStateCurrent[i];

        if ( phase != mSystemMgr->phase(system.asString()) )
          continue;

        ImGui::PushID(system.asCString());

        const bool deleteRequested = ImGui::SmallButton("-##systemDel");

        ImGui::PopID(); // systemId

        ImGui::SameLine();
        ImGui::Selectable(system.asCString(), false,
                          ImGuiSelectableFlags_AllowItemOverlap);

        if ( deleteRequested == true )
          mSystemsStateCurrent.removeIndex(i, nullptr);
      }

      ImGui::EndTable(); // SystemsTable
    }

    ImGui::PopID(); // phase
  };

  if ( ImGui::BeginTabBar("SystemManagerUiTabs") == false )
    return;

  if ( ImGui::BeginTabItem("Logic") )
  {
    systemsShow(Phase::Logic);
    ImGui::EndTabItem(); // Logic
  }

  if ( ImGui::BeginTabItem("Render") )
  {
    systemsShow(Phase::Render);
    ImGui::EndTabItem(); // Render
  }

  ImGui::EndTabBar(); // SystemManagerUiTabs

  ImGui::End(); //  Systems
}

} // namespace cqde::types
