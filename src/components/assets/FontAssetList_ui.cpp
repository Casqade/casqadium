#include <cqde/components/assets/FontAssetList.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>

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

  const auto fontAddPopupLabel {"##fontAddPopup"};

  static ui::IdSelector fontSelector {
    "Font ID", fontAddPopupLabel };

  if ( ImGui::SmallButton("+##fontAdd") )
    ImGui::OpenPopup(fontAddPopupLabel);

  auto& fontManager = registry.ctx().get <FontAssetManager> ();

  fontSelector.selectPopup(
    fontManager.assetIdList(),
    [&fonts = fonts] ( const auto& fontId )
    {
      fonts.insert(fontId);
    },
    [&fonts = fonts] ( const auto& fontId )
    {
      return std::find(
        fonts.begin(),
        fonts.end(),
        fontId ) == fonts.end();
    });


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
