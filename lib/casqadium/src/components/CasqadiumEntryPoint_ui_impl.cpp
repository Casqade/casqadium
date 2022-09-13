#include <cqde/components/CasqadiumEntryPoint.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
CasqadiumEntryPoint::ui_edit_props(
  const entt::entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::EntityManager;
  using types::SystemManager;


  static ui::StringFilter systemFilter {"System ID"};
  static ui::StringFilter entityFilter {"Entity ID"};

  if ( ImGui::CollapsingHeader("Systems to activate",
                               ImGuiTreeNodeFlags_DefaultOpen) == false )
  {
    auto& systemManager = registry.ctx().at <SystemManager> ();

    if ( ImGui::SmallButton("+##systemAdd") )
      ImGui::OpenPopup("##systemAddPopup");

    if ( ImGui::BeginPopup("##systemAddPopup") )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      systemFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      bool systemsFound {};

      for ( const auto& systemId : systemManager.systems() )
      {
        if ( systemFilter.query(systemId.str()) == false )
          continue;

        systemsFound = true;

        if ( ImGui::Selectable(systemId.str().c_str(), false) )
        {
          systemsToEnable.insert(systemId);
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

    if ( ImGui::BeginTable( "SystemsToEnableList", 1, tableFlags) == false )
      return;

    ImGui::TableNextColumn();

    for ( auto iter = systemsToEnable.begin();
          iter != systemsToEnable.end();
          ++iter )
    {
      ImGui::PushID(std::distance(systemsToEnable.begin(), iter));

      if ( ImGui::SmallButton("-##systemDel") )
        iter = systemsToEnable.erase(iter);

      if ( iter == systemsToEnable.end() )
      {
        ImGui::PopID();
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(format("{}###", iter->str()).c_str(),
                        false, flags);
    }

    ImGui::EndTable(); // SystemsToEnableList
  }

  if ( ImGui::CollapsingHeader("Entities to activate",
                               ImGuiTreeNodeFlags_DefaultOpen) == false )
  {
    auto& entityManager = registry.ctx().at <EntityManager> ();

    if ( ImGui::SmallButton("+##entityAdd") )
      ImGui::OpenPopup("##entityAddPopup");

    if ( ImGui::BeginPopup("##entityAddPopup") )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      bool entitiesFound {};

      for ( const auto& entityId : entityManager.entities() )
      {
        if ( entityFilter.query(entityId.str()) == false )
          continue;

        entitiesFound = true;

        if ( ImGui::Selectable(entityId.str().c_str(), false) )
        {
          entitiesToEnableUpdate.insert(entityId);
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      if ( entitiesFound == false )
        ImGui::Text("No entities matching filter");

      ImGui::EndPopup(); // entityAddPopup
    }

    ImGui::Separator();

    const auto tableFlags = ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_ScrollY;

    if ( ImGui::BeginTable( "EntitiesToEnableUpdateList", 1, tableFlags) == false )
      return;

    ImGui::TableNextColumn();

    for ( auto iter = entitiesToEnableUpdate.begin();
          iter != entitiesToEnableUpdate.end();
          ++iter )
    {
      ImGui::PushID(std::distance(entitiesToEnableUpdate.begin(), iter));

      if ( ImGui::SmallButton("-##entityDel") )
        iter = entitiesToEnableUpdate.erase(iter);

      if ( iter == entitiesToEnableUpdate.end() )
      {
        ImGui::PopID();
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(format("{}###", iter->str()).c_str(),
                        false, flags);
    }

    ImGui::EndTable(); // EntitiesToEnableUpdateList
  }

  if ( ImGui::CollapsingHeader("Entities to activate input",
                               ImGuiTreeNodeFlags_DefaultOpen) == false )
  {
    auto& entityManager = registry.ctx().at <EntityManager> ();

    if ( ImGui::SmallButton("+##entityAdd") )
      ImGui::OpenPopup("##entityAddPopup");

    if ( ImGui::BeginPopup("##entityAddPopup") )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      bool entitiesFound {};

      for ( const auto& entityId : entityManager.entities() )
      {
        if ( entityFilter.query(entityId.str()) == false )
          continue;

        entitiesFound = true;

        if ( ImGui::Selectable(entityId.str().c_str(), false) )
        {
          entitiesToEnableInput.insert(entityId);
          ImGui::CloseCurrentPopup();
          break;
        }
      }

      if ( entitiesFound == false )
        ImGui::Text("No entities matching filter");

      ImGui::EndPopup(); // entityAddPopup
    }

    ImGui::Separator();

    const auto tableFlags = ImGuiTableFlags_ScrollX |
                            ImGuiTableFlags_ScrollY;

    if ( ImGui::BeginTable( "EntitiesToEnableInputList", 1, tableFlags) == false )
      return;

    ImGui::TableNextColumn();

    for ( auto iter = entitiesToEnableInput.begin();
          iter != entitiesToEnableInput.end();
          ++iter )
    {
      ImGui::PushID(std::distance(entitiesToEnableInput.begin(), iter));

      if ( ImGui::SmallButton("-##entityDel") )
        iter = entitiesToEnableInput.erase(iter);

      if ( iter == entitiesToEnableInput.end() )
      {
        ImGui::PopID();
        break;
      }

      const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                          ImGuiSelectableFlags_AllowItemOverlap;

      ImGui::SameLine();
      ImGui::Selectable(format("{}###", iter->str()).c_str(),
                        false, flags);
    }
    ImGui::EndTable(); // EntitiesToEnableInputList
  }
}

} // namespace cqde::compos
