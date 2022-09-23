#include <cqde/components/assets/FontAssetUnloadList.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
FontAssetUnloadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::FontAssetManager;

  if ( ImGui::CollapsingHeader("Fonts to unload", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto fontList = registry.ctx().at <FontAssetManager> ().assetIdList();

  static ui::StringFilter fontFilter {"Font ID"};

  if ( ImGui::SmallButton("+##fontAdd") )
    ImGui::OpenPopup("##fontAddPopup");

  if ( ImGui::BeginPopup("##fontAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    fontFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool fontsFound {};

    for ( const auto& fontId : fontList )
    {
      if ( fontFilter.query(fontId.str()) == false )
        continue;

      if ( std::find(fontsToUnload.begin(), fontsToUnload.end(),
                     fontId) != fontsToUnload.end() )
        continue;

      fontsFound = true;

      if ( ImGui::Selectable(fontId.str().c_str(), false) )
      {
        fontsToUnload.insert(fontId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( fontsFound == false )
      ImGui::Text("No fonts matching filter");

    ImGui::EndPopup(); // fontAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "FontsToUnloadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = fontsToUnload.begin();
        iter != fontsToUnload.end();
        ++iter )
  {
    ImGui::PushID(std::distance(fontsToUnload.begin(), iter));

    if ( ImGui::SmallButton("-##fontDel") )
      iter = fontsToUnload.erase(iter);

    if ( iter == fontsToUnload.end() )
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

  ImGui::EndTable(); // FontsToUnloadList
}

} // namespace cqde::compos
