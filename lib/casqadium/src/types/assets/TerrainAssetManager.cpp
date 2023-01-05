#include <cqde/types/assets/AssetManager-inl.hpp>
#include <cqde/types/Terrain.hpp>

#include <cqde/json_helpers.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{

template <>
Json::Value
AssetManager <Terrain>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// terrain DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  auto& referenceGrid = reference["grid"];
  referenceGrid = Json::objectValue;
  referenceGrid.setComment("// terrain 'grid' must be a JSON object"s,
                            Json::CommentPlacement::commentBefore);

  referenceGrid["rows"] = ValueType::uintValue;
  referenceGrid["rows"].setComment("// terrain grid 'rows' must be a JSON unsigned integer"s,
                                    Json::CommentPlacement::commentBefore);

  referenceGrid["columns"] = ValueType::uintValue;
  referenceGrid["columns"].setComment("// terrain grid 'columns' must be a JSON unsigned integer"s,
                                      Json::CommentPlacement::commentBefore);


  auto& referenceHeight = reference["height"];
  referenceHeight = Json::objectValue;
  referenceHeight.setComment("// terrain 'height' must be a JSON object"s,
                              Json::CommentPlacement::commentBefore);

  referenceHeight["min"] = ValueType::realValue;
  referenceHeight["min"].setComment("// terrain height 'min' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  referenceHeight["max"] = ValueType::realValue;
  referenceHeight["max"].setComment("// terrain height 'max' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);


  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// terrain 'path' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <Terrain>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// terrain DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <Terrain>::initPlaceholders()
{
  insert(null_id, std::make_shared <Terrain> ());
}

template <>
void
AssetManager <Terrain>::parseJsonEntryImpl(
  const Json::Value& entry,
  const AssetId& id )
{
  jsonValidateObject( entry, AssetJsonDbEntryReference() );

  mAssetsProperties[id]["grid"] = entry["grid"];
  mAssetsProperties[id]["height"] = entry["height"];
}

template <>
AssetManager <Terrain>::AssetHandle
AssetManager <Terrain>::loadImpl(
  const AssetId& id,
  const AssetPath& path ) const
{
  LOG_INFO("Loading terrain '{}'", path.string());

  auto terrain = std::make_shared <Terrain> ();

  auto& jsonGrid = mAssetsProperties.at(id)["grid"];
  auto& jsonHeight = mAssetsProperties.at(id)["height"];

  terrain->grid.rows = jsonGrid["rows"].asUInt();
  terrain->grid.columns = jsonGrid["columns"].asUInt();

  terrain->heightRange.first = jsonHeight["min"].asFloat();
  terrain->heightRange.second = jsonHeight["max"].asFloat();

  if ( terrain->grid.rows < 2 ||
       terrain->grid.columns < 2 )
  {
    LOG_ERROR("Failed to load terrain '{}' data from '{}': "
              "grid size must be at least 2x2 cells",
              id.str(), path.string());
    return {};
  }

  std::fstream fileStream {};

  try
  {
    fileStream = fileOpen(path, std::ios::in);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load terrain '{}' data from '{}': {}",
              id.str(), path.string(), e.what());
    return {};
  }

  const auto terrainSize = terrain->grid.rows * terrain->grid.columns;

  terrain->data.clear();
  terrain->data.reserve(terrainSize);

  for ( int32_t i = 0;
        i < terrainSize;
        ++i )
  {
    float buf {};

    try
    {
      fileStream >> buf;
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to load terrain '{}' data from '{}': "
                "failed to read element {}: {}",
                id.str(), path.string(), i, e.what());
      return {};
    }

    if ( buf < terrain->heightRange.first ||
         buf > terrain->heightRange.second )
    {
      LOG_ERROR("Failed to load terrain '{}' data from '{}': "
                "element {} is out of height range",
                id.str(), path.string(), i);
      return {};
    }

    terrain->data.push_back(buf);
  }

  return terrain;
}

template <>
void
AssetManager <Terrain>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Freeing terrain");
  handle.reset();
}

template <>
void
AssetManager <Terrain>::ui_show_preview(
  const AssetId& terrainId,
  entt::registry& )
{
  using fmt::format;

  const auto assetStatus = status(terrainId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(StatusAsColor(assetStatus)),
                       "%s", StatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  if ( mAssets.at(terrainId).path != MemoryResidentPath )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({terrainId});
      load({terrainId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", mAssets.at(terrainId).path.string().c_str());
}

template <>
void
AssetManager <Terrain>::ui_show(
  Json::Value& entry ) const
{
  if ( ImGui::CollapsingHeader("Grid size", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_AlwaysClamp;

    auto& entryGrid = entry["grid"];

    int32_t rows = entryGrid["rows"].asUInt();
    int32_t columns = entryGrid["columns"].asUInt();

    if (  ImGui::DragInt("##hfieldGridRows", &rows, 1.0f, 2,
                          std::numeric_limits <int>::max(),
                          "Rows: %d", flags) )
      entryGrid["rows"] = rows;

    if (  ImGui::DragInt("##hfieldGridColumns", &columns, 1.0f, 2,
                          std::numeric_limits <int>::max(),
                          "Columns: %d", flags) )
      entryGrid["columns"] = columns;
  }

  if ( ImGui::CollapsingHeader("Height range", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    const auto flags = ImGuiSliderFlags_NoRoundToFormat |
                       ImGuiSliderFlags_AlwaysClamp;

    auto& entryHeight = entry["height"];

    auto heightMin = entryHeight["min"].asFloat();
    auto heightMax = entryHeight["max"].asFloat();

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    if (  ImGui::DragFloat("##hfieldHeightMin", &heightMin, 0.1f,
                           std::numeric_limits <float>::lowest(),
                           heightMax, "Min: %.1f", flags) )
      entryHeight["min"] = heightMin;

    if (  ImGui::DragFloat("##hfieldHeightMax", &heightMax,
                           0.1f, heightMin,
                           std::numeric_limits <float>::max(),
                           "Max: %.1f", flags) )
      entryHeight["max"] = heightMax;

    ImGui::PopItemWidth();
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##terrainPath", &path) )
      entry["path"] = path;
  }
}

template class
AssetManager <Terrain>;

} // namespace cqde::types

