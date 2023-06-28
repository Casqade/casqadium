#include <cqde/components/SystemList.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

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

  auto& systemManager = registry.ctx().get <SystemManager> ();

  const auto systemAddPopupLabel {"##systemAddPopup"};

  static ui::IdSelector systemSelector {
    "System ID", systemAddPopupLabel };

  if ( ImGui::SmallButton("+##systemAdd") )
    ImGui::OpenPopup(systemAddPopupLabel);

  systemSelector.selectPopup(
    systemManager.systems(),
    [&systems = systems] ( const auto& systemId )
    {
      systems.insert(systemId);
    },
    [&systems = systems] ( const auto& systemId )
    {
      return std::find(
        systems.begin(),
        systems.end(),
        systemId ) == systems.end();
    });


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
