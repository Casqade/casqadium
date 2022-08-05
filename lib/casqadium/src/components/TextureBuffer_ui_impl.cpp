#include <cqde/components/TextureBuffer.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
TextureBuffer::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  if ( ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  for ( auto iter = textures.begin();
        iter < textures.end();
        ++iter )
  {
    ImGui::PushID(std::distance(textures.begin(), iter));

    if ( ImGui::SmallButton("-##textureDel") )
      iter = textures.erase(iter);

    ImGui::PopID();

    if ( iter == textures.end() )
      break;

    ImGui::SameLine();
    ImGui::Selectable(iter->str().c_str() );

    if ( ImGui::IsItemActive() == true &&
         ImGui::IsItemHovered() == false )
    {
      auto iter_next = iter + (ImGui::GetMouseDragDelta(0).y < 0.0f
                               ? -1
                               : 1);

      if ( iter_next >= textures.begin() && iter_next < textures.end() )
      {
        std::swap(*iter, *iter_next);
        ImGui::ResetMouseDragDelta();
      }
    }
  }

//  for ( size_t i = 0;
//        i < textures.size();
//        ++i )
//  {
//    ImGui::PushID(i);

//    const auto texture = textures.at(i);

//    ImGui::TreeNodeEx(texture.str().c_str(),
//                      ImGuiTreeNodeFlags_Bullet |
//                      ImGuiTreeNodeFlags_Leaf |
//                      ImGuiTreeNodeFlags_NoTreePushOnOpen );

//    if ( ImGui::BeginDragDropSource() )
//    {
//      ImGui::SetDragDropPayload("textureIndex", &i, sizeof(size_t));
//      ImGui::EndDragDropSource();
//    }

//    if ( ImGui::BeginDragDropTarget() )
//    {
//      if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("textureIndex") )
//      {
//        IM_ASSERT(payload->DataSize == sizeof(size_t));
//        size_t i_target = *(const size_t*) payload->Data;
//        std::swap(textures.at(i), textures.at(i_target));
//      }
//      ImGui::EndDragDropTarget();
//    }

//    ImGui::SameLine();
//    if ( ImGui::SmallButton("-##textureDel") )
//      textures.erase(textures.begin() + i);

//    ImGui::PopID();
//  }

  using types::TextureAssetManager;

  const auto assets = registry.ctx().at <TextureAssetManager> ().assetIdList();

  if ( assets.empty() == true )
  {
    ImGui::Text("No textures in TextureAssetManager");
    return;
  }

  static TextureId textureToAdd {assets.front()};

  if ( ImGui::Button("+##textureAdd") )
    textures.push_back(textureToAdd);

  ImGui::SameLine();
  if ( ImGui::BeginCombo("##textureId", textureToAdd.str().c_str()) )
  {
    for ( const auto& asset : assets )
    {
      const bool selected = (textureToAdd == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        textureToAdd = asset;
    }
    ImGui::EndCombo();
  }
}

} // namespace cqde::compos
