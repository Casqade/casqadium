#include <cqde/components/assets/TextStringAssetLoadList.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TextStringAssetLoadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TextStringAssetManager;

  if ( ImGui::CollapsingHeader("Text strings to load", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto textStringList = registry.ctx().at <TextStringAssetManager> ().assetIdList();

  static ui::StringFilter textStringFilter {"String ID"};

  if ( ImGui::SmallButton("+##textStringAdd") )
    ImGui::OpenPopup("##textStringAddPopup");

  if ( ImGui::BeginPopup("##textStringAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    textStringFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool textStringsFound {};

    for ( const auto& textStringId : textStringList )
    {
      if ( textStringFilter.query(textStringId.str()) == false )
        continue;

      if ( std::find(textToLoad.begin(), textToLoad.end(),
                     textStringId) != textToLoad.end() )
        continue;

      textStringsFound = true;

      if ( ImGui::Selectable(textStringId.str().c_str(), false) )
      {
        textToLoad.insert(textStringId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( textStringsFound == false )
      ImGui::Text("No text string matching filter");

    ImGui::EndPopup(); // textStringAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "TextStringsToLoadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = textToLoad.begin();
        iter != textToLoad.end();
        ++iter )
  {
    ImGui::PushID(std::distance(textToLoad.begin(), iter));

    if ( ImGui::SmallButton("-##textStringDel") )
      iter = textToLoad.erase(iter);

    if ( iter == textToLoad.end() )
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

  ImGui::EndTable(); // TextStringsToLoadList
}

} // namespace cqde::compos
