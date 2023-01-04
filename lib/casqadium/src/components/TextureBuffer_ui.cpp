#include <cqde/components/TextureBuffer.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TextureBuffer::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto textureList = registry.ctx().get <TextureAssetManager> ().assetIdList();

  static ui::StringFilter textureFilter {"Texture ID"};

  if ( ImGui::SmallButton("+##textureAdd") )
    ImGui::OpenPopup("##textureAddPopup");

  if ( ImGui::BeginPopup("##textureAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    textureFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool texturesFound {};

    for ( const auto& textureId : textureList )
    {
      if ( textureFilter.query(textureId.str()) == false )
        continue;

      texturesFound = true;

      if ( ImGui::Selectable(textureId.str().c_str(), false) )
      {
        textures.push_back(textureId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( texturesFound == false )
      ImGui::Text("No textures matching filter");

    ImGui::EndPopup(); // textureAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "TexturesList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = textures.begin();
        iter < textures.end();
        ++iter )
  {
    ImGui::PushID(std::distance(textures.begin(), iter));

    if ( ImGui::SmallButton("-##textureDel") )
      iter = textures.erase(iter);

    if ( iter == textures.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(format("{}###", iter->str()).c_str(),
                      false, flags);

    static auto iter_dragged = textures.end();

    if ( ImGui::IsItemHovered() == true )
      ImGui::SetTooltip("Drag to reorder");

    if ( ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) == true &&
         ImGui::IsMouseDown(ImGuiMouseButton_Left) == true )
    {
      auto mouseClickPos = ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left];

      if (  mouseClickPos.x > ImGui::GetItemRectMin().x &&
            mouseClickPos.x < ImGui::GetItemRectMax().x )
      {
        if ( iter_dragged != iter &&
             iter_dragged >= textures.begin() &&
             iter_dragged < textures.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged != textures.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = textures.end();

    ImGui::PopID();
  }

  ImGui::EndTable(); // TexturesList
}

} // namespace cqde::compos
