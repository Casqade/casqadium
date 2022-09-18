#include <cqde/types/PackageManager.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value manifestReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  root["entry_point"] = ValueType::stringValue;
  root["entry_point"].setComment("// entry_point must be a JSON string"s,
                                 Json::CommentPlacement::commentBefore);

  root["load_order"] = ValueType::arrayValue;
  root["load_order"].setComment("// load_order must be a JSON array"s,
                                Json::CommentPlacement::commentBefore);

  root["load_order"].append(ValueType::stringValue);
  root["load_order"].begin()->setComment("// load_order element must be a JSON string"s,
                                          Json::CommentPlacement::commentBefore);

  return root;
}();

void
PackageManager::Validate(
  const Json::Value& packagesManifest )
{
  jsonValidateObject(packagesManifest, manifestReference);
}

void
PackageManager::setRootPath(
  const path& packagesRoot )
{
  mPackagesRootPath = packagesRoot;
}

void
PackageManager::parseManifest()
{
  using fmt::format;

  Json::Value manifest {};

  LOG_TRACE("Parsing packages manifest '{}'",
            manifestPath().string());

  try
  {
    manifest = fileParse(manifestPath());
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse packages manifest ({})",
              e.what()));
  }

  LOG_TRACE("Validating packages manifest '{}'",
            manifestPath().string());

  std::set <PackageId> uniquePackages {};

  try
  {
    Validate(manifest);

    for ( const auto& packageId : manifest["load_order"] )
    {
      if ( uniquePackages.insert(packageId.asString()).second == false )
        throw std::runtime_error(
          format("load_order contains more than one instance of '{}'",
                  packageId.asString()));
    }
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate packages manifest '{}': {}",
              manifestPath().string(), e.what()));
  }

  mEntryPoint = manifest["entry_point"].asString();

  mPackages.reserve(uniquePackages.size());

  for ( const auto& packageId : manifest["load_order"] )
    mPackages.push_back( {packageId.asString()} );
}

void
PackageManager::load(
  entt::registry& registry )
{
  using fmt::format;

  mPackages.clear();

  LOG_TRACE("Loading packages from '{}'",
            manifestPath().string());

  try
  {
    parseManifest();

    std::set <PackageId> loadedPackages {};

    for ( auto& package : mPackages )
    {
      const auto packageId = package.id();

      package.setRootPath(mPackagesRootPath / packageId.str());
      package.parseManifest();

      for ( const auto& dependency : package.dependencies() )
        if ( loadedPackages.count(dependency) == 0 )
          throw std::runtime_error(
            format("Dependency '{}' must be loaded before package '{}'",
                    dependency.str(), packageId.str()));

      package.load(registry);
      loadedPackages.emplace(packageId);
    }
  }
  catch ( const std::exception& e )
  {
    mPackages.clear();
    throw std::runtime_error(
      format("Failed to load packages ({})",
              e.what()));
  }
}

void
PackageManager::unload()
{
  mPackages.clear();
}

const Package*
PackageManager::package(
  const PackageId& id ) const
{
  for ( const auto& package : mPackages )
    if ( package.id() == id )
      return &package;

  return nullptr;
}

std::vector <PackageId>
PackageManager::packages() const
{
  std::vector <PackageId> result {};
  result.reserve(mPackages.size());

  for ( const auto& package : mPackages )
    result.push_back(package.id());

  return result;
}

PackageManager::path
PackageManager::rootPath() const
{
  return mPackagesRootPath;
}

PackageManager::path
PackageManager::manifestPath() const
{
  return mPackagesRootPath / "packages.json";
}

} // namespace cqde::types
