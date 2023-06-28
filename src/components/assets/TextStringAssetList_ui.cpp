#include <cqde/components/assets/TextStringAssetList.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TextStringAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TextStringAssetManager;

  if ( ImGui::CollapsingHeader("Text strings", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto textStringAddPopupLabel {"##textStringAddPopup"};

  static ui::IdSelector textStringSelector {
    "String ID", textStringAddPopupLabel };

  if ( ImGui::SmallButton("+##textStringAdd") )
    ImGui::OpenPopup(textStringAddPopupLabel);

  auto& textManager = registry.ctx().get <TextStringAssetManager> ();

  textStringSelector.selectPopup(
    textManager.assetIdList(),
    [&text = text] ( const auto& textStringId )
    {
      text.insert(textStringId);
    },
    [&text = text] ( const auto& textStringId )
    {
      return std::find(
        text.begin(),
        text.end(),
        textStringId ) == text.end();
    });


  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "TextStringsList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = text.begin();
        iter != text.end();
        ++iter )
  {
    ImGui::PushID(std::distance(text.begin(), iter));

    if ( ImGui::SmallButton("-##textStringDel") )
      iter = text.erase(iter);

    if ( iter == text.end() )
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

  ImGui::EndTable(); // TextStringsList
}

} // namespace cqde::compos
