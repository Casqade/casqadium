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
    jsonValidateObject(manifest, manifestReference);
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

  if ( mTitle == null_id.str() )
    throw std::runtime_error(
      format("Packages with title 'null' are forbidden"));

  for ( const auto& dependency : manifest["dependencies"])
    mDependencies.emplace(dependency.asString());
}

void
Package::load(
  entt::registry& registry )
{
  LOG_DEBUG("Loading package '{}'", mManifestPath.string());

  parseManifest(mManifestPath);

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
    auto fileStream = fileOpen(path, std::ios::out | std::ios::trunc);
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
  const auto packageRootPath = mManifestPath.parent_path();

  switch (type)
  {
    case ContentType::Manifest:
      return mManifestPath;

    case ContentType::Entities:
      return packageRootPath / "entities.json";

    case ContentType::Input:
      return packageRootPath / "input.json";

    case ContentType::Fonts:
      return packageRootPath / "fonts.json";

    case ContentType::Geometry:
      return packageRootPath / "geometry.json";

    case ContentType::Textures:
      return packageRootPath / "textures.json";

    case ContentType::Text:
      return packageRootPath / "text.json";
  }
}

} // namespace cqde::types
