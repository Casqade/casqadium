#include <cqde/types/PrefabManager.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/logger.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>


namespace cqde::types
{

const static Json::Value prefabDbReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// prefab DB root must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& prefab = reference["cqde_json_anykey"];
  prefab = ValueType::objectValue;
  prefab.setComment("// prefab DB entry must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  return reference;
}();

void
PrefabManager::Validate(
  const Json::Value& prefabDbJson )
{
  jsonValidateObject(prefabDbJson, prefabDbReference);

  for ( const auto& prefabId : prefabDbJson.getMemberNames() )
    EntityManager::Validate(prefabDbJson[prefabId]);
}

void
PrefabManager::load(
  const path& prefabDbPath )
{
  using fmt::format;

  if ( fileExists(prefabDbPath) == false )
    return;

  Json::Value prefabDbJson {};

  LOG_TRACE("Parsing prefab DB '{}'", prefabDbPath.string());

  try
  {
    prefabDbJson = fileParse(prefabDbPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse prefab DB ({})", e.what()));
  }

  LOG_TRACE("Validating prefab DB");

  try
  {
    Validate(prefabDbJson);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate prefab DB: {}", e.what()));
  }

  try
  {
    deserialize(prefabDbJson);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to load prefab DB '{}': {}",
              prefabDbPath.string(), e.what()));
  }
}

void
PrefabManager::clear()
{
  mPrefabs.clear();
}

void
PrefabManager::deserialize(
  const Json::Value& prefabDbJson )
{
  LOG_DEBUG("Deserializing prefab DB");

  for ( const auto& prefabId : prefabDbJson.getMemberNames() )
    mPrefabs[prefabId] = prefabDbJson[prefabId];
}

Json::Value
PrefabManager::serialize() const
{
  return mPrefabs;
}

Json::Value
PrefabManager::get(
  const PrefabId& id ) const
{
  return mPrefabs.get(id.str(), {});
}

} // namespace cqde::types
