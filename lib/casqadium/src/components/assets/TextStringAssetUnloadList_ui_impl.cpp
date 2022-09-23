#include <cqde/components/assets/TextStringAssetUnloadList.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TextStringAssetUnloadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TextStringAssetManager;

  if ( ImGui::CollapsingHeader("Text strings to unload", ImGuiTreeNodeFlags_DefaultOpen) == false )
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

      if ( std::find(textToUnload.begin(), textToUnload.end(),
                     textStringId) != textToUnload.end() )
        continue;

      textStringsFound = true;

      if ( ImGui::Selectable(textStringId.str().c_str(), false) )
      {
        textToUnload.insert(textStringId);
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

  if ( ImGui::BeginTable( "TextStringsToUnloadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = textToUnload.begin();
        iter != textToUnload.end();
        ++iter )
  {
    ImGui::PushID(std::distance(textToUnload.begin(), iter));

    if ( ImGui::SmallButton("-##textStringDel") )
      iter = textToUnload.erase(iter);

    if ( iter == textToUnload.end() )
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

  ImGui::EndTable(); // TextStringsToUnloadList
}

} // namespace cqde::compos
