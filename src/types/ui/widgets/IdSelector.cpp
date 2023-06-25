#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::ui
{

IdSelector::IdSelector(
  const std::string& filterHint,
  const std::string& comboLabel )
  : mIdFilter{filterHint}
  , mComboLabel{comboLabel}
{}

bool
IdSelector::select(
  const entt::registry& registry,
  identifier& selectedId,
  const std::vector <identifier>& idList )
{
  bool selected {};

  if ( ImGui::BeginCombo(mComboLabel.c_str(), selectedId.str().c_str()) )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    mIdFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto& id : idList )
    {
      if ( mIdFilter.query(id.str()) == false )
        continue;

      if ( ImGui::Selectable(id.str().c_str(), selectedId == id) )
      {
        selectedId = id;
        selected = true;
      }
    }
    ImGui::EndCombo();
  }

  return selected;
}

} // namespace cqde::ui
