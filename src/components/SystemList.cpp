#include <cqde/components/SystemList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value systemsEnablerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& systems = root["systems"];
  systems = ValueType::arrayValue;
  systems.setComment("// 'systems' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  systems.append(ValueType::stringValue);
  systems.begin()->setComment("// 'systems' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
SystemList::serialize() const
{
  Json::Value json {};

  auto& jsonSystems = json["systems"];
  jsonSystems = Json::arrayValue;

  for ( const auto& system : systems )
    jsonSystems.append(system.str());

  return json;
}

void
SystemList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, systemsEnablerJsonReference);

  auto& comp = registry.emplace_or_replace <SystemList> (entity);

  for ( const auto& system : json["systems"] )
    comp.systems.insert(system.asString());
}

} // namespace cqde::compos
