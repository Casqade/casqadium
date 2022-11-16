#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
FontAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::FontAssetManager;

  if ( ImGui::CollapsingHeader("Fonts", ImGuiTreeNodeFlags_DefaultOpen) == false )
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

      if ( std::find(fonts.begin(), fonts.end(),
                     fontId) != fonts.end() )
        continue;

      fontsFound = true;

      if ( ImGui::Selectable(fontId.str().c_str(), false) )
      {
        fonts.insert(fontId);
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

  if ( ImGui::BeginTable( "FontList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = fonts.begin();
        iter != fonts.end();
        ++iter )
  {
    ImGui::PushID(std::distance(fonts.begin(), iter));

    if ( ImGui::SmallButton("-##fontDel") )
      iter = fonts.erase(iter);

    if ( iter == fonts.end() )
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

  ImGui::EndTable(); // FontList
}

} // namespace cqde::compos
