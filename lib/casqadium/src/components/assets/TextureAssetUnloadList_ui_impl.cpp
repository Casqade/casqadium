#include <cqde/components/assets/TextureAssetUnloadList.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
TextureAssetUnloadList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Textures to unload", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto textureList = registry.ctx().at <TextureAssetManager> ().assetIdList();

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

      if ( std::find(texturesToUnload.begin(), texturesToUnload.end(),
                     textureId) != texturesToUnload.end() )
        continue;

      texturesFound = true;

      if ( ImGui::Selectable(textureId.str().c_str(), false) )
      {
        texturesToUnload.insert(textureId);
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

  if ( ImGui::BeginTable( "TexturesToUnloadList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = texturesToUnload.begin();
        iter != texturesToUnload.end();
        ++iter )
  {
    ImGui::PushID(std::distance(texturesToUnload.begin(), iter));

    if ( ImGui::SmallButton("-##textureDel") )
      iter = texturesToUnload.erase(iter);

    if ( iter == texturesToUnload.end() )
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

  ImGui::EndTable(); // TexturesToUnloadList
}

} // namespace cqde::compos
