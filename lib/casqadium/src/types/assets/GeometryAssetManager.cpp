#include <cqde/types/assets/AssetManager-inl.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/conversion/json_glm_vec3.hpp>

#include <glm/vec3.hpp>

#include <json/value.h>
#include <json/reader.h>

#include <imgui.h>


namespace cqde::types
{

using vertexBuffer = std::vector <glm::vec3>;

template <>
Json::Value
AssetManager <vertexBuffer>::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::arrayValue;
  reference.setComment("// geometry DB entry must be a JSON array"s,
                       Json::CommentPlacement::commentBefore);

  reference.append(ValueType::arrayValue);

  Json::Value& vertex = *reference.begin();
  vertex.setComment("// geometry vertex must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  vertex.append(ValueType::realValue);
  vertex.begin()->setComment("// geometry vertex element must be a JSON float"s,
                              Json::CommentPlacement::commentBefore);

  return reference;
}

template <>
void
AssetManager <vertexBuffer>::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// geometry DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

template <>
void
AssetManager <vertexBuffer>::initPlaceholders()
{
  const auto geometry = std::make_shared <vertexBuffer> (
  vertexBuffer
  {
    {-0.5f, 0.5f, 0.0f},
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f}
  });

  insert(null_id, geometry);
}

template <>
void
AssetManager <vertexBuffer>::parseAssetDb(
  const Json::Value& geometryDb,
  const path& dbPath )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : geometryDb.getMemberNames() )
  {
    try
    {
      jsonValidateArray(geometryDb[id], AssetJsonDbEntryReference());

      mAssets[id].path = dbPath;
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Unloaded;

      continue;
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to validate JSON entry for geometry '{}' ('{}'): {}",
                id, dbPath.string(), e.what());

      mAssets[id].path = path{};
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }
  }
}

template <>
void
AssetManager <vertexBuffer>::load(
  const std::unordered_set <AssetId, identifier_hash>& ids )
{
  using fmt::format;

  mThreadPool.push(
  [this, ids] ( const int32_t threadId )
  {
    std::multimap <AssetPath, AssetId> geometryPaths {};

    mAssetsMutex.lock();

    for ( const auto& id : ids )
    {
      CQDE_ASSERT_DEBUG(id.str().empty() == false, continue);

      const AssetStatus assetStatus = status(id);

      if ( assetStatus == AssetStatus::Undefined )
      {
        LOG_ERROR("Can't load unknown geometry '{}'", id.str());
        continue;
      }

      else if ( assetStatus >= AssetStatus::Loading )
        continue;

      const AssetPath assetPath = mAssets.at(id).path;

      CQDE_ASSERT_DEBUG(assetPath.empty() == false, continue);
      CQDE_ASSERT_DEBUG(assetPath.string() != MemoryResidentPath, continue);

      mAssets.at(id).status = AssetStatus::Loading;

      geometryPaths.insert({assetPath, id});
    }

    mAssetsMutex.unlock();

    Json::Value geometryDb {};

    bool dbHasErrors = false;

    AssetPath pathPrev {};
    for ( const auto& [path, id] : geometryPaths )
    {
      LOG_DEBUG("Loading geometry '{}' ('{}')",
                id.str(), path.string());

      if ( path == pathPrev && dbHasErrors == true )
      {
        LOG_ERROR("Failed to load geometry '{}' (errors in DB '{}')",
                  id.str(), path.string());
        continue;
      }

      if ( path != pathPrev )
      {
        pathPrev = path;
        geometryDb.clear();

        LOG_TRACE("Parsing geometry DB '{}'", path.string());

        try
        {
          geometryDb = fileParse(path);

          if ( geometryDb.isObject() == false )
            throw std::runtime_error(
              format( "JSON root in '{}' must be an object",
                      path.string()));
        }
        catch ( const std::exception& e )
        {
          dbHasErrors = true;
          LOG_ERROR("Failed to parse geometry DB ({})", e.what());
          LOG_ERROR("Failed to load geometry '{}' (invalid DB '{}')",
                    id.str(), path.string());
          continue;
        }
        dbHasErrors = false;
      }

      std::shared_ptr <vertexBuffer> handle {};

      const Json::Value geometryEntry = geometryDb[id.str()];

      try
      {
        jsonValidateArray(geometryEntry, AssetJsonDbEntryReference());

        handle = std::make_shared <vertexBuffer> ();

        for ( const auto& vertexJson : geometryEntry )
        {
          glm::vec3 vertex {};
          vertex << vertexJson;
          handle->push_back(vertex);
        }
      }
      catch ( const std::exception& e )
      {
        handle.reset();
        LOG_ERROR("Failed to load geometry '{}' ('{}'): {}",
                  id.str(), path.string(), e.what());
      }

      std::lock_guard guard(mAssetsMutex);

      if ( status(id) <= AssetStatus::Unloaded )
        continue;

      auto& assetEntry = mAssets.at(id);

      if ( handle != nullptr )
      {
        assetEntry.status = AssetStatus::Loaded;
        assetEntry.handle = std::move(handle);

        continue;
      }

      assetEntry.status = AssetStatus::Error;
      assetEntry.handle = nullptr;
    }
  });
}

template <>
void
AssetManager <vertexBuffer>::unloadImpl(
  AssetHandle& handle ) const
{
  LOG_TRACE("Freeing geometry");
  handle.reset();
}

template <>
void
AssetManager <vertexBuffer>::ui_show_preview(
  const AssetId& geometryId,
  entt::registry& )
{
  if ( ImGui::CollapsingHeader("Preview", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("WIP");

//  todo: impl

//  auto drawList = ImGui::GetWindowDrawList();

//  drawList->AddPolyline();
}

template <>
void
AssetManager <vertexBuffer>::ui_show(
  Json::Value& entry ) const
{
  using fmt::format;

  const auto flags = ImGuiSliderFlags_NoRoundToFormat;

  size_t vertexCount {};

  for ( auto& vertex : entry )
  {
    ImGui::PushID(++vertexCount);

    float vertexX = vertex[0].asFloat();
    if ( ImGui::DragFloat("##vertexX", &vertexX, 0.01f,
                          0.0f, 0.0f, "X: %.2f", flags) )
      vertex[0] = vertexX;

    float vertexY = vertex[1].asFloat();
    if ( ImGui::DragFloat("##vertexY", &vertexY, 0.01f,
                          0.0f, 0.0f, "Y: %.2f", flags) )
      vertex[1] = vertexY;

    float vertexZ = vertex[2].asFloat();
    if ( ImGui::DragFloat("##vertexZ", &vertexZ, 0.01f,
                          0.0f, 0.0f, "Z: %.2f", flags) )
      vertex[2] = vertexZ;

    ImGui::Separator();
    ImGui::PopID();
  }

  if ( ImGui::SmallButton("+##vertexAdd") )
  {
    Json::Value vertex {};
    vertex.append(Json::realValue);
    vertex.append(Json::realValue);
    vertex.append(Json::realValue);

    entry.append(vertex);
  }
}

template class
AssetManager <vertexBuffer>;

} // namespace cqde::types