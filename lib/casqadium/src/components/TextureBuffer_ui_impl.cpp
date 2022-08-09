#include <cqde/components/TextureBuffer.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

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
  if ( ImGui::BeginCombo("##textureId", textureToAdd.str().c_str(),
                         ImGuiComboFlags_HeightLargest) )
  {
    for ( const auto& asset : assets )
    {
      const bool selected = (textureToAdd == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        textureToAdd = asset;
    }
    ImGui::EndCombo();
  }

  ImGui::Separator();

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

    ImGui::SameLine();
    ImGui::Selectable(format("{}###", iter->str()).c_str());

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

//  for ( size_t i = 0;
//        i < textures.size();
//        ++i )
//  {
//    ImGui::PushID(i);

//    const auto texture = textures.at(i);

//    if ( ImGui::SmallButton("-##textureDel") )
//      textures.erase(textures.begin() + i);

//    if ( i >= textures.size() )
//      break;

//    ImGui::SameLine();
//    ImGui::TreeNodeEx(format("{}###{}", texture.str(), i).c_str(),
//                      ImGuiTreeNodeFlags_Leaf |
//                      ImGuiTreeNodeFlags_NoTreePushOnOpen );

//    if ( ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip |
//                                    ImGuiDragDropFlags_SourceNoDisableHover) )
//    {
//      LOG_INFO("src {}", i);
//      ImGui::SetDragDropPayload("textureIndex", &i, sizeof(size_t));
//      ImGui::EndDragDropSource();
//    }

//    if ( ImGui::GetDragDropPayload() != nullptr &&
//         ImGui::BeginDragDropTarget() )
//    {
//      LOG_INFO("tgt {}", i);
//      auto payload = ImGui::AcceptDragDropPayload("textureIndex",
//                                                  ImGuiDragDropFlags_AcceptBeforeDelivery);
//      if ( payload != nullptr )
//      {
//        IM_ASSERT(payload->DataSize == sizeof(size_t));

//        size_t i_source = *(const size_t*) payload->Data;
//        std::swap(textures.at(i), textures.at(i_source));

//        if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) == false )
//        {
//          ImGui::SetDragDropPayload("textureIndex", &i, sizeof(size_t));
//        }
//      }
//      ImGui::EndDragDropTarget();
//    }
//    ImGui::PopID();
//  }
}

} // namespace cqde::compos
