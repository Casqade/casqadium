#include <cqde/types/PackageManager.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

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
  root["load_order"].append(ValueType::stringValue);
  root["load_order"].setComment("// load_order must be a JSON array"s,
                                Json::CommentPlacement::commentBefore);

  root["load_order"].begin()->setComment("// load_order element must be a JSON string"s,
                                          Json::CommentPlacement::commentBefore);

  return root;
}();

void
PackageManager::parseRoot()
{
  Json::Value packages {};

  LOG_TRACE("Parsing packages root '{}'", mPackagesRoot.string());

  try
  {
    packages = fileParse(mPackagesRoot);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to parse packages root ({})",
                                    e.what()));
  }

  try
  {
    jsonValidateObject(packages, rootReference);

    std::set <PackageId> existingPackages {};

    for ( const auto& packageId : packages["load_order"] )
    {
      if ( existingPackages.count(packageId.asString()) > 0 )
        throw std::runtime_error(format("load_order contains more than one instance of '{}'",
                                        packageId.asString()));

      existingPackages.emplace(packageId.asString());
    }
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to parse packages root '{}': {}",
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
  mPackages.clear();
  mPackagesRoot = packagesRoot;

  LOG_TRACE("Loading packages from '{}'", mPackagesRoot.string());

  try
  {
    parseRoot();

    std::set <PackageId> loadedPackages {};

    for ( auto& [packageId, package] : mPackages )
    {
      const auto packagePath = mPackagesRoot.parent_path() / packageId.str() / "manifest.json";
      package.load(registry, packagePath);

      for ( const auto& dependency : package.dependencies() )
        if ( loadedPackages.count(dependency) == 0 )
          throw std::runtime_error(format("Dependency '{}' must be loaded before package '{}'",
                                          dependency.str(), packageId.str()));

      auto& fonts = registry.ctx().at <FontAssetManager> ();
      fonts.parseAssetDbFile(packagePath.parent_path() / "fonts.json");

      auto& textures = registry.ctx().at <TextureAssetManager> ();
      textures.parseAssetDbFile(packagePath.parent_path() / "textures.json");

      auto& text = registry.ctx().at <TextStringAssetManager> ();
      text.parseAssetDbFile(packagePath.parent_path() / "text.json");

      loadedPackages.emplace(packageId);
    }
  }
  catch ( const std::exception& e )
  {
    mPackages.clear();
    throw std::runtime_error(format("Failed to load packages ({})",
                                    e.what()));
  }
}

std::filesystem::path
PackageManager::rootPath() const
{
  return mPackagesRoot;
}

} // namespace cqde::types
