#include <cqde/components/EntityList.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

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

  const auto entityAddPopupLabel {"##entityAddPopup"};

  static ui::IdSelector entitySelector {
    "Entity ID", entityAddPopupLabel };

  if ( ImGui::SmallButton("+##entityAdd") )
    ImGui::OpenPopup(entityAddPopupLabel);

  auto& entityManager = registry.ctx().get <EntityManager> ();

  entitySelector.selectPopup(
    entityManager.entities(),
    [&entities = entities] ( const auto& entityId )
    {
      entities.insert(entityId);
    },
    [&entities = entities] ( const auto& id )
    {
      return entities.count(id) == 0;
    });

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
