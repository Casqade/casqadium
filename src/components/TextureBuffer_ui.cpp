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
  using ui::StringFilter;
  using types::TextureAssetManager;

  if ( ImGui::CollapsingHeader("Texture ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    static StringFilter textureFilter {"Texture ID"};

    const auto assets = registry.ctx().get <TextureAssetManager> ().assetIdList();

    if ( ImGui::BeginCombo("##textureId", texture.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      textureFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& asset : assets )
      {
        if ( textureFilter.query(asset.str()) == false )
          continue;

        const bool selected = (texture == asset);

        if ( ImGui::Selectable(asset.str().c_str(), selected) )
          texture = asset;
      }
      ImGui::EndCombo();
    }
  }

  if ( ImGui::CollapsingHeader("UV", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    using fmt::format;

    const auto flags = ImGuiSliderFlags_NoRoundToFormat;

    bool updateNeeded {};
    size_t vertexCount {};
    int32_t vertexToDelete {-1};

    for ( auto& vertex : uvBuffer )
    {
      ImGui::PushID(++vertexCount);

      const auto width = ImGui::GetContentRegionAvail().x * 0.5f;

      ImGui::PushItemWidth(width);

      updateNeeded |= ImGui::DragFloat(
        "##vertexU", &vertex.x, 0.01f,
        0.0f, 0.0f, "U: %.2f", flags );

      ImGui::SameLine();

      updateNeeded |= ImGui::DragFloat(
        "##vertexV", &vertex.y, 0.01f,
        0.0f, 0.0f, "V: %.2f", flags );

      ImGui::PopItemWidth();

      if ( ImGui::SmallButton("-##vertexDel") )
        vertexToDelete = vertexCount - 1;

      ImGui::Separator();
      ImGui::PopID();
    }

    if ( vertexToDelete >= 0 )
    {
      updateNeeded = true;
      uvBuffer.erase(uvBuffer.begin() + vertexToDelete);
    }

    if ( ImGui::SmallButton("+##vertexAdd") )
    {
      updateNeeded = true;
      uvBuffer.push_back({});
    }

    if ( updateNeeded == true )
      update();
  }
}

} // namespace cqde::compos
