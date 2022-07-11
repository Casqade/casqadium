#include <cqde/types/PackageManager.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value rootReference =
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
PackageManager::parseRoot()
{
  using fmt::format;

  Json::Value packages {};

  LOG_TRACE("Parsing packages root '{}'",
            mPackagesRoot.string());

  try
  {
    packages = fileParse(mPackagesRoot);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse packages root ({})",
              e.what()));
  }

  LOG_TRACE("Validating packages root '{}'",
            mPackagesRoot.string());

  try
  {
    jsonValidateObject(packages, rootReference);

    std::set <PackageId> existingPackages {};

    for ( const auto& packageId : packages["load_order"] )
    {
      if ( existingPackages.count(packageId.asString()) > 0 )
        throw std::runtime_error(
          format("load_order contains more than one instance of '{}'",
                  packageId.asString()));

      existingPackages.emplace(packageId.asString());
    }
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate packages root '{}': {}",
              mPackagesRoot.string(), e.what()));
  }

  mEntryPoint = packages["entry_point"].asString();

  for ( const auto& packageId : packages["load_order"] )
    mPackages.emplace(packageId.asString(), Package{});
}

void
PackageManager::load(
  const std::filesystem::path& packagesRoot,
  entt::registry& registry )
{
  using fmt::format;

  mPackages.clear();
  mPackagesRoot = packagesRoot;

  LOG_TRACE("Loading packages from '{}'",
            mPackagesRoot.string());

  try
  {
    parseRoot();

    std::set <PackageId> loadedPackages {};

    for ( auto& [packageId, package] : mPackages )
    {
      const auto packageManifest = mPackagesRoot.parent_path() / packageId.str() / "manifest.json";
      package.parseManifest(packageManifest);

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

std::filesystem::path
PackageManager::rootPath() const
{
  return mPackagesRoot;
}

} // namespace cqde::types
