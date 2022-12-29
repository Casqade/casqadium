#include <cqde/components/EntityList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value entityListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& entities = root["entities"];
  entities = ValueType::arrayValue;
  entities.setComment("// 'entities' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  entities.append(ValueType::stringValue);
  entities.begin()->setComment("// 'entities' element must be a JSON string"s,
                                Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
EntityList::serialize() const
{
  Json::Value json {};

  auto& jsonEntities = json["entities"];
  jsonEntities = Json::arrayValue;

  for ( const auto& entity : entities )
    jsonEntities.append(entity.str());

  return json;
}

void
EntityList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, entityListJsonReference);

  auto& comp = registry.emplace_or_replace <EntityList> (entity);

  for ( const auto& entity : json["entities"] )
    comp.entities.insert(entity.asString());
}

} // namespace cqde::compos
