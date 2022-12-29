#include <cqde/components/SystemList.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
SystemList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::SystemManager;

  if ( ImGui::CollapsingHeader("Systems", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto systemList = registry.ctx().at <SystemManager> ().systems();

  static ui::StringFilter systemFilter {"System ID"};

  if ( ImGui::SmallButton("+##systemAdd") )
    ImGui::OpenPopup("##systemAddPopup");

  if ( ImGui::BeginPopup("##systemAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    systemFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool systemsFound {};

    for ( const auto& systemId : systemList )
    {
      if ( systemFilter.query(systemId.str()) == false )
        continue;

      if ( std::find(systems.begin(), systems.end(),
                     systemId) != systems.end() )
        continue;

      systemsFound = true;

      if ( ImGui::Selectable(systemId.str().c_str(), false) )
      {
        systems.insert(systemId);
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

  if ( ImGui::BeginTable( "SystemList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = systems.begin();
        iter != systems.end();
        ++iter )
  {
    ImGui::PushID(std::distance(systems.begin(), iter));

    if ( ImGui::SmallButton("-##systemDel") )
      iter = systems.erase(iter);

    if ( iter == systems.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(iter->str().c_str(), false, flags);

    ImGui::PopID();
  }

  ImGui::EndTable(); // SystemList
}

} // namespace cqde::compos
