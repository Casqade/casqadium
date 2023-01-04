#include <cqde/types/Package.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PrefabManager.hpp>

#include <json/value.h>
#include <json/writer.h>


namespace cqde::types
{

const static Json::Value manifestReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value manifest = ValueType::objectValue;
  manifest.setComment("// manifest root must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  manifest["title"] = ValueType::stringValue;
  manifest["title"].setComment("// title must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  manifest["description"] = ValueType::stringValue;
  manifest["description"].setComment("// description must be a JSON string"s,
                                      Json::CommentPlacement::commentBefore);

  manifest["dependencies"] = ValueType::arrayValue;
  manifest["dependencies"].append(ValueType::stringValue);
  manifest["dependencies"].setComment("// dependencies list must be a JSON array"s,
                                      Json::CommentPlacement::commentBefore);

  manifest["dependencies"].begin()->setComment("// dependencies list element must be a JSON string"s,
                                                Json::CommentPlacement::commentBefore);

  manifest["version"] = ValueType::stringValue;
  manifest["version"].setComment("// version must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  return manifest;
}();

void
Package::Validate(
  const Json::Value& packageManifest )
{
  jsonValidateObject(packageManifest, manifestReference);
}

Package::Package(
  const PackageId& id )
  : mId{id}
{}

Json::Value
Package::serialize() const
{
  using Json::ValueType;

  Json::Value json {};

  json["title"] = mTitle;
  json["description"] = mDescription;
  json["version"] = mVersion;

  json["dependencies"] = ValueType::arrayValue;

  for ( const auto& dependency : mDependencies )
    json["dependencies"].append(dependency.str());

  return json;
}

void
Package::deserialize(
  const Json::Value& manifest )
{
  using fmt::format;

  try
  {
    Validate(manifest);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate package '{}' manifest: {}",
              mId.str(), e.what()));
  }

  mTitle = manifest["title"].asString();
  mDescription = manifest["description"].asString();
  mVersion = manifest["version"].asString();

  for ( const auto& dependency : manifest["dependencies"])
    mDependencies.emplace(dependency.asString());
}

void
Package::setRootPath(
  const path& rootPath )
{
  mRootPath = rootPath;
}

void
Package::parseManifest()
{
  using fmt::format;

  const auto manifestPath = contentPath(ContentType::Manifest);

  Json::Value manifest {};

  try
  {
    manifest = fileParse(manifestPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse package '{}' manifest: {}",
              mId.str(), e.what()));
  }

  deserialize(manifest);
}

void
Package::load(
  entt::registry& registry )
{
  const auto manifestPath = contentPath(ContentType::Manifest);

  LOG_DEBUG("Loading package '{}' ('{}')", mId.str(), manifestPath.string());

  auto& audio = registry.ctx().get <AudioAssetManager> ();
  audio.parseAssetDbFile(contentPath(ContentType::Audio));

  auto& fonts = registry.ctx().get <FontAssetManager> ();
  fonts.parseAssetDbFile(contentPath(ContentType::Fonts));

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();
  geometry.parseAssetDbFile(contentPath(ContentType::Geometry));

  auto& terrain = registry.ctx().get <TerrainAssetManager> ();
  terrain.parseAssetDbFile(contentPath(ContentType::Terrain));

  auto& textures = registry.ctx().get <TextureAssetManager> ();
  textures.parseAssetDbFile(contentPath(ContentType::Textures));

  auto& text = registry.ctx().get <TextStringAssetManager> ();
  text.parseAssetDbFile(contentPath(ContentType::Text));

  auto& input = registry.ctx().get <InputManager> ();
  input.load(contentPath(ContentType::Input));

  auto& prefabs = registry.ctx().get <PrefabManager> ();
  prefabs.load(contentPath(ContentType::Prefabs));

  auto& entityManager = registry.ctx().get <EntityManager> ();
  entityManager.load( contentPath(ContentType::Entities),
                      mId, registry );
}

bool
Package::save(
  const ContentType type,
  const Json::Value& data ) const
{
  const auto path = contentPath(type);

  if ( fileExists(path) == false &&
       data.empty() == true )
    return true;

  std::string contentTypeName {};

  switch (type)
  {
    case ContentType::Manifest:
      contentTypeName = "manifest";
      break;

    case ContentType::Entities:
      contentTypeName = "entity registry";
      break;

    case ContentType::Prefabs:
      contentTypeName = "prefab database";
      break;

    case ContentType::Input:
      contentTypeName = "input config";
      break;

    case ContentType::Audio:
      contentTypeName = "audio database";
      break;

    case ContentType::Fonts:
      contentTypeName = "font database";
      break;

    case ContentType::Geometry:
      contentTypeName = "geometry database";
      break;

    case ContentType::Terrain:
      contentTypeName = "terrain database";
      break;

    case ContentType::Textures:
      contentTypeName = "texture database";
      break;

    case ContentType::Text:
      contentTypeName = "text database";
      break;
  }

  LOG_TRACE("Writing package '{}' {} to '{}'",
            mId.str(), contentTypeName, path.string());

  try
  {
    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    auto fileStream = fileOpen(path, streamFlags);
    fileStream << Json::writeString(jsonWriter(), data);
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("Failed to write package '{}' {} to '{}': {}",
              mId.str(), contentTypeName, path.string(), e.what());
    return false;
  }

  return true;
}

PackageId
Package::id() const
{
  return mId;
}

std::string
Package::title() const
{
  return mTitle;
}

std::string
Package::description() const
{
  return mDescription;
}

std::string
Package::version() const
{
  return mVersion;
}
std::set <PackageId>
Package::dependencies() const
{
  return mDependencies;
}

Package::path
Package::contentPath(
  const ContentType type ) const
{
  return mRootPath / ContentFileName(type);
}

std::string
Package::ContentFileName(
  const ContentType type )
{
  switch (type)
  {
    case ContentType::Manifest:
      return "manifest.json";

    case ContentType::Entities:
      return "entities.json";

    case ContentType::Prefabs:
      return "prefabs.json";

    case ContentType::Audio:
      return "audio.json";

    case ContentType::Input:
      return "input.json";

    case ContentType::Fonts:
      return "fonts.json";

    case ContentType::Geometry:
      return "geometry.json";

    case ContentType::Terrain:
      return "terrain.json";

    case ContentType::Textures:
      return "textures.json";

    case ContentType::Text:
      return "text.json";

    default:
      CQDE_ASSERT_DEBUG(false, return "unhandled enum value in Package::ContentFileName");
  }
}

} // namespace cqde::types
