#include <cqde/components/InteractionProbe.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
InteractionProbe::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  using ui::StringFilter;
  using types::EntityManager;

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Listener entity", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto entityList = registry.ctx().at <EntityManager> ().entities();

    static StringFilter entityFilter {"Entity ID"};

    if ( ImGui::BeginCombo("##listenerId", listenerId.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& entityId : entityList )
      {
        if ( entityFilter.query(entityId.str()) == false )
          continue;

        const bool selected = (listenerId == entityId);

        if ( ImGui::Selectable(entityId.str().c_str(), selected) )
          listenerId = entityId;
      }
      ImGui::EndCombo();
    }
  }
}

} // namespace cqde
