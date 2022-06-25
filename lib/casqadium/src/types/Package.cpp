#include <cqde/types/Package.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>

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
Package::parseManifest()
{
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
Package::load(
  entt::registry& registry,
  const std::filesystem::path& manifestPath )
{
  LOG_DEBUG("Loading package '{}'", manifestPath.string());

  mManifestPath = manifestPath;
  parseManifest();
}

std::set <PackageId>
Package::dependencies() const
{
  return mDependencies;
}

} // namespace cqde::types
