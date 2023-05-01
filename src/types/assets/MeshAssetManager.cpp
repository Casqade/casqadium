#include <cqde/types/assets/MeshAssetManager.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/assert.hpp>
#include <cqde/logger.hpp>
#include <cqde/static_strings.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>

#include <glad/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <json/value.h>
#include <json/writer.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <fstream>


namespace cqde::types
{

MeshAssetManager::MeshAssetManager()
{
  initPlaceholders();
}

MeshAssetManager::~MeshAssetManager()
{
  for ( auto& [id, entry] : mAssets )
    unload({id});
}

Json::Value
MeshAssetManager::AssetJsonDbEntryReference()
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// mesh DB entry must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["path"] = ValueType::stringValue;
  reference["path"].setComment("// mesh 'path' must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return reference;
}

void
MeshAssetManager::Validate(
  const Json::Value& json )
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// mesh DB must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& asset = reference["cqde_json_anykey"];
  asset = AssetJsonDbEntryReference();

  jsonValidateObject(json, reference);
}

void
MeshAssetManager::initPlaceholders()
{
  const std::vector <glm::vec3> vertices
  {
    {-0.5f, -0.5f, 0.0f},
    {0.5f, -0.5f, 0.0f},
    {0.5f, 0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f},
  };

  const std::vector <glm::vec2> texCoords
  {
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
  };

  const std::vector <uint32_t> indices
  {
    0, 1, 2,
    2, 3, 0,
  };

  mMeshes.resize(vertices.size(), indices.size());

  std::memcpy(
    mMeshes.verticesBuffer.data(),
    vertices.data(),
    BufferSize(vertices) );

  std::memcpy(
    mMeshes.texCoordsBuffer.data(),
    texCoords.data(),
    BufferSize(texCoords) );

  std::memcpy(
    mMeshes.elementBuffer.data(),
    indices.data(),
    BufferSize(indices) );

  const auto quad = std::make_shared <Mesh> ();
  quad->vertexCount = vertices.size();
  quad->elementCount = indices.size();

  insert(null_id, quad);
}

void
MeshAssetManager::parseAssetDb(
  const Json::Value& meshDb,
  const Path& dbPath )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : meshDb.getMemberNames() )
  {
    try
    {
      jsonValidateObject(meshDb[id], AssetJsonDbEntryReference());

      mAssets[id].path = (dbPath.parent_path() / meshDb[id]["path"].asString()).u8string();
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Unloaded;
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("Failed to validate JSON entry for asset '{}' ('{}'): {}",
                id, dbPath.string(), e.what());

      mAssets[id].path = Path{};
      mAssets[id].handle = {};
      mAssets[id].status = AssetStatus::Error;

      continue;
    }
  }
}

void
MeshAssetManager::parseAssetDbFile(
  const Path& dbPath )
{
  using fmt::format;

  if ( fileExists(dbPath) == false )
    return;

  Json::Value assetDb {};

  LOG_TRACE("Parsing mesh asset DB '{}'", dbPath.string());

  try
  {
    assetDb = fileParse(dbPath);

    if ( assetDb.isObject() == false )
      throw std::runtime_error(
        format("mesh asset DB root in '{}' must be a JSON object",
                dbPath.string()));
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse mesh asset DB ({})",
              e.what()));
  }

  parseAssetDb(assetDb, dbPath);
}

void
MeshAssetManager::load(
  const AssetIdList& ids )
{
  using fmt::format;

  for ( const auto& id : ids )
  {
    CQDE_ASSERT_DEBUG(id.str().empty() == false, continue);

    mAssetsMutex.lock();

    const AssetStatus assetStatus = status(id);

    if ( assetStatus == AssetStatus::Undefined )
    {
      mAssetsMutex.unlock();
      LOG_ERROR("Can't load unknown mesh '{}'", id.str());
      continue;
    }

    else if ( assetStatus >= AssetStatus::Loading )
    {
      mAssetsMutex.unlock();
      continue;
    }

    const Path assetPath = mAssets.at(id).path;

    CQDE_ASSERT_DEBUG(assetPath.string().empty() == false, continue);
    CQDE_ASSERT_DEBUG(assetPath.string() != MemoryResidentPath(), continue);

    mAssets.at(id).status = AssetStatus::Loading;

    LOG_DEBUG("Loading mesh '{}' ('{}')",
              id.str(), assetPath.string());

    mAssetsMutex.unlock();

    AssetHandle resource = loadImpl(id, assetPath);

    std::lock_guard guard(mAssetsMutex);

    if ( status(id) <= AssetStatus::Unloaded )
    {
      resource.reset();
      continue;
    }

    auto& assetEntry = mAssets.at(id);

    if ( resource != nullptr )
    {
      assetEntry.status = AssetStatus::Loaded;
      assetEntry.handle = std::move(resource);

      continue;
    }

    assetEntry.status = AssetStatus::Error;
    assetEntry.handle = nullptr;
  }
}

MeshAssetManager::AssetHandle
MeshAssetManager::loadImpl(
  const AssetId& id,
  const Path& path )
{
  using fmt::format;

  LOG_INFO("Loading mesh '{}'", path.string());

  try
  {
    const auto meshJson = fileParse(path);

    if ( meshJson.isObject() == false )
      throw std::runtime_error(
        format("mesh root in '{}' must be a JSON object",
               path.string()));

    if ( meshJson.isMember("asset") == true )
    {
      Assimp::Importer importer {};

      const auto scene = importer.ReadFile(
        path.string(), aiProcess_FlipUVs );

      if ( scene == nullptr ||
           scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
           scene->mRootNode == nullptr )
      {
        throw std::runtime_error(
          importer.GetErrorString());
      }

      if ( scene->mRootNode->mNumMeshes == 0 )
        throw std::runtime_error(
          format("scene '{}' does not contain any meshes",
                 path.string()));

      if ( scene->mRootNode->mNumMeshes > 1 )
        throw std::runtime_error(
          format("only 1 mesh is supported for gltf scene ('{}')",
                 path.string()));

      const auto mesh = scene->mMeshes[0];

      if ( mesh->HasTextureCoords(0) == false )
        throw std::runtime_error(
          format("mesh in '{}' does not contain texture coordinates",
                 path.string()));

      std::vector <glm::vec3> vertices {};
      std::vector <glm::vec2> uvCoordinates {};
      std::vector <uint32_t> indices {};

      for ( size_t i = 0;
            i < mesh->mNumVertices;
            ++i )
      {
        const auto vertex = mesh->mVertices[i];

        vertices.push_back(
        {
          vertex.x,
          vertex.y,
          vertex.z,
        });

        const auto texCoord = mesh->mTextureCoords[0][i];

        uvCoordinates.push_back(
        {
          texCoord.x,
          texCoord.y,
        });
      }

      for ( size_t faceIndex = 0;
            faceIndex < mesh->mNumFaces;
            ++faceIndex )
      {
        const auto face = mesh->mFaces[faceIndex];

        for ( size_t i = 0;
              i < face.mNumIndices;
              ++i )
          indices.push_back(face.mIndices[i]);
      }

      if ( vertices.size() == 0 ||
           uvCoordinates.size() == 0 )
        throw std::runtime_error(
          format("mesh in '{}' does not contain any vertices",
                 path.string()));

      if ( indices.size() == 0 )
        throw std::runtime_error(
          format("mesh in '{}' does not contain any indices",
                 path.string()));

      const auto quad = std::make_shared <Mesh> ();

      quad->vertexCount = vertices.size();
      quad->elementCount = indices.size();

      fitMesh(*quad);

      mMeshes.resize(
        quad->firstVertexIndex + quad->vertexCount,
        quad->firstElementIndex + quad->elementCount );

      const auto verticesData = static_cast <glm::vec3*> (mMeshes.verticesBuffer.data());
      const auto texCoordsData = static_cast <glm::vec2*> (mMeshes.texCoordsBuffer.data());
      const auto indicesData = static_cast <uint32_t*> (mMeshes.elementBuffer.data());

      std::memcpy(
        verticesData + quad->firstVertexIndex,
        vertices.data(),
        BufferSize(vertices));

      std::memcpy(
        texCoordsData + quad->firstVertexIndex,
        uvCoordinates.data(),
        types::BufferSize(uvCoordinates));

      std::memcpy(
        indicesData + quad->firstElementIndex,
        indices.data(),
        BufferSize(indices));

      return quad;
    }
    else if ( meshJson.isMember("quad") == true )
    {
      const auto& quadJson = meshJson["quad"];

      if ( quadJson.isArray() == false )
        throw std::runtime_error(
          format("quad '{}' must be a JSON array",
                 path.string()));

      if ( quadJson.size() != 4 )
        throw std::runtime_error(
          format("quad array in '{}' must contain 4 UV coordinates",
                 path.string()));

      std::vector <glm::vec2> uvCoordinates {};
      uvCoordinates.reserve(4);

      for ( const auto& uvCoordinateJson : quadJson )
      {
        glm::vec2 uvCoordinate {};
        uvCoordinate << uvCoordinateJson;
        uvCoordinates.push_back(uvCoordinate);
      }

      const auto nullQuad = mAssets.at(null_id).handle;

      const auto quad = std::make_shared <Mesh> ();

      quad->vertexCount = nullQuad->vertexCount;
      quad->elementCount = nullQuad->elementCount;

      fitMesh(*quad);

      mMeshes.resize(
        quad->firstVertexIndex + quad->vertexCount,
        quad->firstElementIndex + quad->elementCount );

      const auto verticesData = static_cast <glm::vec3*> (mMeshes.verticesBuffer.data());
      const auto texCoordsData = static_cast <glm::vec2*> (mMeshes.texCoordsBuffer.data());
      const auto indicesData = static_cast <uint32_t*> (mMeshes.elementBuffer.data());

      std::memcpy(
        verticesData + quad->firstVertexIndex,
        verticesData,
        sizeof(glm::vec3) * nullQuad->vertexCount);

      std::memcpy(
        texCoordsData + quad->firstVertexIndex,
        uvCoordinates.data(),
        sizeof(glm::vec2) * nullQuad->vertexCount);

      std::memcpy(
        indicesData + quad->firstElementIndex,
        indicesData,
        sizeof(uint32_t) * nullQuad->elementCount);

      return quad;
    }
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to load mesh '{}': {}",
              id.str(), e.what());
  }

  return {};
}

void
MeshAssetManager::unload(
  const std::unordered_set <AssetId, identifier_hash>& ids )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& id : ids )
  {
    CQDE_ASSERT_DEBUG(mAssets.count(id) > 0, continue);

    const auto assetPath = mAssets.at(id).path;

    LOG_DEBUG("Unloading mesh '{}' ('{}')",
              id.str(), assetPath.string());

    mAssets.at(id).handle.reset();

    mAssets.at(id).status = AssetStatus::Unloaded;
  }
}

void
MeshAssetManager::insert(
  const AssetId& id,
  const AssetHandle res )
{
  std::lock_guard guard(mAssetsMutex);

  mAssets[id].path = MemoryResidentPath();
  mAssets[id].handle = res;
  mAssets[id].status = AssetStatus::Loaded;
}

void
MeshAssetManager::remove(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  if ( mAssets.count(id) == 0 )
    return;

  unload({id});

  mAssets.erase(id);
}

void
MeshAssetManager::clear(
  const bool keepMemoryResidents )
{
  std::lock_guard guard(mAssetsMutex);

  for ( const auto& [id, entry] : mAssets )
    if ( keepMemoryResidents == true &&
         entry.path != MemoryResidentPath() )
      unload({id});

  std::vector <std::pair <AssetId, AssetEntry>> assetsInMemory {};

  if ( keepMemoryResidents == true )
    for ( const auto& [id, entry] : mAssets )
      if ( entry.path == MemoryResidentPath() )
        assetsInMemory.push_back({id, entry});

  mAssets.clear();

  if ( keepMemoryResidents == false )
    return;

  for ( const auto& [id, entry] : assetsInMemory )
    mAssets.insert({id, entry});
}

AssetStatus
MeshAssetManager::status(
  const AssetId& id ) const
{
  std::lock_guard guard(mAssetsMutex);

  if ( mAssets.count(id) == 0 )
    return AssetStatus::Undefined;

  return mAssets.at(id).status;
}

std::vector <MeshAssetManager::AssetId>
MeshAssetManager::assetIdList() const
{
  std::lock_guard guard(mAssetsMutex);

  std::vector <AssetId> list {};
  list.reserve(mAssets.size());

  for ( const auto& [id, entry] : mAssets )
    list.push_back(id);

  return list;
}

std::shared_ptr <Mesh>
MeshAssetManager::try_get(
  const AssetId& id ) const
{
  using namespace cqde::literals;

  std::lock_guard guard(mAssetsMutex);

  switch (status(id))
  {
    case AssetStatus::Loading:
      return {};

    case AssetStatus::Loaded:
      return mAssets.at(id).handle;

    default:
      return mAssets.at(null_id).handle;
  }
}

std::shared_ptr <Mesh>
MeshAssetManager::get(
  const AssetId& id )
{
  std::lock_guard guard(mAssetsMutex);

  if ( status(id) == AssetStatus::Unloaded )
    load({id});

  return try_get(id);
}

uint32_t
MeshAssetManager::vaoId() const
{
  return mMeshes.vao.id();
}

void
MeshAssetManager::fitMesh(
  Mesh& newMesh ) const
{
  std::vector <Mesh> meshes {};
  meshes.reserve(mAssets.size());

  {
    std::lock_guard guard(mAssetsMutex);

    for ( auto&& [meshId, mesh] : mAssets )
      if ( mesh.handle != nullptr )
        meshes.push_back(*mesh.handle);
  }

  std::sort( meshes.begin(), meshes.end(),
  [] ( const Mesh& lhs, const Mesh& rhs )
  {
    return lhs.firstVertexIndex < rhs.firstVertexIndex;
  });

  for ( const auto& mesh : meshes )
  {
    if (  mesh.firstVertexIndex >= newMesh.firstVertexIndex + newMesh.vertexCount &&
          mesh.firstElementIndex >= newMesh.firstElementIndex + newMesh.elementCount )
      return;

    newMesh.firstVertexIndex = mesh.firstVertexIndex + mesh.vertexCount;
    newMesh.firstElementIndex = mesh.firstElementIndex + mesh.elementCount;
  }
}

void
MeshAssetManager::ui_show_preview(
  const AssetId& meshId,
  entt::registry& )
{
  using fmt::format;

  const auto assetStatus = status(meshId);

  if ( ImGui::CollapsingHeader("Status", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(AssetStatusAsColor(assetStatus)),
                       "%s", AssetStatusAsString(assetStatus).c_str());

  if ( assetStatus == AssetStatus::Undefined )
    return;

  const auto meshPath = mAssets.at(meshId).path.string();

  if ( meshPath != MemoryResidentPath() )
  {
    ImGui::SameLine();
    if ( ImGui::Button("Reload") )
    {
      unload({meshId});
      load({meshId});
    }
  }

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
    ImGui::Text("%s", meshPath.c_str());

  if ( meshPath == MemoryResidentPath() )
    return;

  if ( ImGui::CollapsingHeader("UV", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static AssetId previousMeshId {};
  static Json::Value meshJson {};
  static bool errorsInMesh {};

  if ( meshId != previousMeshId )
  {
    meshJson.clear();
    errorsInMesh = false;
    previousMeshId = meshId;
  }

  if ( ImGui::Button("Reload quad UV") )
  {
    meshJson.clear();
    errorsInMesh = false;
  }

  if ( meshJson.empty() == true &&
       errorsInMesh == false )
  {
    if ( fileExists(meshPath) == true )
    {
      try
      {
        meshJson = fileParse(meshPath);
      }
      catch ( const std::exception& e )
      {
        errorsInMesh = true;

        LOG_ERROR( "Failed to parse mesh '{}': {}",
          meshPath, e.what() );
      }
    }
    else
    {
      const std::vector <glm::vec2> texCoords
      {
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
      };

      auto& quadJson = meshJson["quad"];

      for ( const auto& vertex : texCoords )
      {
        Json::Value vertexJson {};
        vertexJson << vertex;
        quadJson.append(vertexJson);
      }
    }
  }

  const bool isQuad =
    errorsInMesh == false &&
    meshJson.isMember("quad") == true &&
    meshJson["quad"].size() == 4;

  if ( isQuad == false )
    ImGui::BeginDisabled();

  if ( ImGui::Button("Save quad UV") )
  {
    auto fileStream = fileOpen(meshPath, std::ios::out | std::ios::trunc);
    fileStream << Json::writeString(jsonWriter(), meshJson);
  }

  if ( isQuad == false )
    ImGui::EndDisabled();

  if ( errorsInMesh )
  {
    ImGui::TextColored(
      ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
      "ERROR: Failed to load mesh" );

    ImGui::TextColored(
      ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
      "Check logs for details" );

    return;
  }

  if ( isQuad == false )
  {
    ImGui::TextColored(
      ImVec4{1.0f, 1.0f, 0.0f, 1.0f},
      "Mesh is not a valid quad" );

    return;
  }

  auto& quadJson = meshJson["quad"];

  const auto flags = ImGuiSliderFlags_NoRoundToFormat;

  size_t vertexCount {};

  for ( auto& vertex : quadJson )
  {
    ImGui::PushID(++vertexCount);

    const auto width = ImGui::GetContentRegionAvail().x * 0.5f;

    ImGui::PushItemWidth(width);

    float vertexU = vertex[0].asFloat();
    if ( ImGui::DragFloat("##vertexU", &vertexU, 0.01f,
                          0.0f, 0.0f, "U: %.2f", flags ) )
      vertex[0] = vertexU;

    float vertexV = vertex[1].asFloat();
    if ( ImGui::DragFloat("##vertexY", &vertexV, 0.01f,
                          0.0f, 0.0f, "V: %.2f", flags) )
      vertex[1] = vertexV;

    ImGui::Separator();
    ImGui::PopID();
  }
}

void
MeshAssetManager::ui_show(
  Json::Value& entry ) const
{
  using fmt::format;

  if ( ImGui::CollapsingHeader("Path", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string path = entry["path"].asString();
    if ( ImGui::InputText("##texturePath", &path) )
      entry["path"] = path;
  }
}

} // namespace cqde::types
