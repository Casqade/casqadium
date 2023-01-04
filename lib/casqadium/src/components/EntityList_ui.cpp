#include <cqde/components/EntityList.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
EntityList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::EntityManager;

  if ( ImGui::CollapsingHeader("Entities", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto entityList = registry.ctx().get <EntityManager> ().entities();

  static ui::StringFilter entityFilter {"Entity ID"};

  if ( ImGui::SmallButton("+##entityAdd") )
    ImGui::OpenPopup("##entityAddPopup");

  if ( ImGui::BeginPopup("##entityAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool entitiesFound {};

    for ( const auto& entityId : entityList )
    {
      if ( entityFilter.query(entityId.str()) == false )
        continue;

      if ( std::find(entities.begin(), entities.end(),
                     entityId) != entities.end() )
        continue;

      entitiesFound = true;

      if ( ImGui::Selectable(entityId.str().c_str(), false) )
      {
        entities.insert(entityId);
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

  if ( ImGui::BeginTable( "EntityList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = entities.begin();
        iter != entities.end();
        ++iter )
  {
    ImGui::PushID(std::distance(entities.begin(), iter));

    if ( ImGui::SmallButton("-##entityDel") )
      iter = entities.erase(iter);

    if ( iter == entities.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(iter->id.str().c_str(), false, flags);

    ImGui::PopID();
  }

  ImGui::EndTable(); // EntityList
}

} // namespace cqde::compos
