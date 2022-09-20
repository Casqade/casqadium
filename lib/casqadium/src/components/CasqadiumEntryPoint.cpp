#include <cqde/components/CasqadiumEntryPoint.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value casqadiumEntryPointJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& systems = root["systems"];
  systems = ValueType::arrayValue;
  systems.setComment("// 'systems' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  systems.append(ValueType::stringValue);
  systems.begin()->setComment("// 'systems' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  auto& entitiesUpdate = root["entities-update"];
  entitiesUpdate = ValueType::arrayValue;
  entitiesUpdate.setComment("// 'entities-update' must be a JSON array"s,
                            Json::CommentPlacement::commentBefore);

  entitiesUpdate.append(ValueType::stringValue);
  entitiesUpdate.begin()->setComment("// 'entities-update' element must be a JSON string"s,
                                    Json::CommentPlacement::commentBefore);

  auto& entitiesInput = root["entities-input"];
  entitiesInput = ValueType::arrayValue;
  entitiesInput.setComment("// 'entities-input' must be a JSON array"s,
                            Json::CommentPlacement::commentBefore);

  entitiesInput.append(ValueType::stringValue);
  entitiesInput.begin()->setComment("// 'entities-input' element must be a JSON string"s,
                                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
CasqadiumEntryPoint::serialize() const
{
  Json::Value json {};

  auto& jsonSystems = json["systems"];
  auto& jsonEntitiesUpdate = json["entities-input"];
  auto& jsonEntitiesInput = json["entities-update"];

  jsonSystems = Json::arrayValue;
  jsonEntitiesUpdate = Json::arrayValue;
  jsonEntitiesInput = Json::arrayValue;

  for ( const auto& systemId : systemsToEnable )
    jsonSystems.append(systemId.str());

  for ( const auto& entityId : entitiesToEnableUpdate )
    jsonEntitiesUpdate.append(entityId.str());

  for ( const auto& entityId : entitiesToEnableInput )
    jsonEntitiesInput.append(entityId.str());

  return json;
}

void
CasqadiumEntryPoint::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, casqadiumEntryPointJsonReference);

  auto& comp = registry.emplace_or_replace <CasqadiumEntryPoint> (entity);

  for ( const auto& systemId : json["systems"] )
    comp.systemsToEnable.insert(systemId.asString());

  for ( const auto& entityId : json["entities-input"] )
    comp.entitiesToEnableUpdate.insert(entityId.asString());

  for ( const auto& entityId : json["entities-update"] )
    comp.entitiesToEnableInput.insert(entityId.asString());
}

} // namespace cqde::compos
