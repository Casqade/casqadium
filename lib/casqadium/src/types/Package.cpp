#include <cqde/types/Package.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/EntityManager.hpp>

#include <json/value.h>


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
Package::parseManifest( const std::filesystem::path& manifestPath )
{
  CQDE_ASSERT_DEBUG(manifestPath.empty() == false, {});

  mManifestPath = manifestPath;

  Json::Value manifest {};

  try
  {
    manifest = fileParse(mManifestPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to parse package manifest ({})",
                                    e.what()));
  }

  try
  {
    jsonValidateObject(manifest, manifestReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to validate package manifest '{}': {}",
                                    mManifestPath.string(), e.what()));
  }

  mTitle = manifest["title"].asString();
  mDescription = manifest["description"].asString();
  mVersion = manifest["version"].asString();

  for ( const auto& dependency : manifest["dependencies"])
    mDependencies.emplace(dependency.asString());
}

void
Package::load( entt::registry& registry )
{
  LOG_DEBUG("Loading package '{}'", mManifestPath.string());

  parseManifest(mManifestPath);

  const auto packageRootPath = mManifestPath.parent_path();

  auto& fonts = registry.ctx().at <FontAssetManager> ();
  fonts.parseAssetDbFile(packageRootPath / "fonts.json");

  auto& textures = registry.ctx().at <TextureAssetManager> ();
  textures.parseAssetDbFile(packageRootPath / "textures.json");

  auto& text = registry.ctx().at <TextStringAssetManager> ();
  text.parseAssetDbFile(packageRootPath / "text.json");

  auto& input = registry.ctx().at <InputManager> ();
  input.load(packageRootPath / "input.json");

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.load( packageRootPath / "entities.json",
                      mTitle, registry );
}

std::set <PackageId>
Package::dependencies() const
{
  return mDependencies;
}

} // namespace cqde::types
