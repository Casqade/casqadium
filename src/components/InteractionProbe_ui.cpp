#include <cqde/components/InteractionProbe.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
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
  using ui::IdSelector;
  using types::EntityManager;

  const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                     ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Listener entity", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto entityList = registry.ctx().get <EntityManager> ().entities();

    static IdSelector entitySelector {"Entity ID", "##listenerId"};

    entitySelector.select(
      registry, listener.id, entityList );
  }
}

} // namespace cqde
