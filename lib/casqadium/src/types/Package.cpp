#include <cqde/types/Package.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/EntityManager.hpp>

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

void
Package::parseManifest(
  const path& manifestPath )
{
  using fmt::format;

  CQDE_ASSERT_DEBUG(manifestPath.empty() == false, {});

  mManifestPath = manifestPath;

  Json::Value manifest {};

  try
  {
    manifest = fileParse(mManifestPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse package manifest ({})",
              e.what()));
  }

  try
  {
    Validate(manifest);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate package manifest '{}': {}",
              mManifestPath.string(), e.what()));
  }

  mTitle = manifest["title"].asString();
  mDescription = manifest["description"].asString();
  mVersion = manifest["version"].asString();

  for ( const auto& dependency : manifest["dependencies"])
    mDependencies.emplace(dependency.asString());
}

void
Package::load(
  entt::registry& registry )
{
  LOG_DEBUG("Loading package '{}'", mManifestPath.string());

  parseManifest(mManifestPath);

  auto& audio = registry.ctx().at <FontAssetManager> ();
  audio.parseAssetDbFile(contentPath(ContentType::Audio));

  auto& fonts = registry.ctx().at <FontAssetManager> ();
  fonts.parseAssetDbFile(contentPath(ContentType::Fonts));

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();
  geometry.parseAssetDbFile(contentPath(ContentType::Geometry));

  auto& textures = registry.ctx().at <TextureAssetManager> ();
  textures.parseAssetDbFile(contentPath(ContentType::Textures));

  auto& text = registry.ctx().at <TextStringAssetManager> ();
  text.parseAssetDbFile(contentPath(ContentType::Text));

  auto& input = registry.ctx().at <InputManager> ();
  input.load(contentPath(ContentType::Input));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.load( contentPath(ContentType::Entities),
                      mTitle, registry );
}

bool
Package::save(
  const ContentType type,
  const Json::Value& data ) const
{
  const auto path = contentPath(type);

  std::string contentTypeName {};

  switch (type)
  {
    case ContentType::Manifest:
      contentTypeName = "manifest";
      break;

    case ContentType::Entities:
      contentTypeName = "entity registry";
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
  return mManifestPath.parent_path() / ContentFileName(type);
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

    case ContentType::Audio:
      return "audio.json";

    case ContentType::Input:
      return "input.json";

    case ContentType::Fonts:
      return "fonts.json";

    case ContentType::Geometry:
      return "geometry.json";

    case ContentType::Textures:
      return "textures.json";

    case ContentType::Text:
      return "text.json";

    default:
      CQDE_ASSERT_DEBUG(false, return "unhandled enum value in Package::ContentFileName");
  }
}

} // namespace cqde::types
