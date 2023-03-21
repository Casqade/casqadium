#include <cqde/components/InteractionListenerColor.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value interactionListenerColorJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& color = root["color"];
  color = ValueType::uintValue;
  color.setComment("// 'color' must be a JSON unsigned integer"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionListenerColor::serialize() const
{
  Json::Value json {};

  json["color"] = color.n;

  return json;
}

void
InteractionListenerColor::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, interactionListenerColorJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionListenerColor> (entity);

  comp.color = json["color"].asUInt();
}

} // namespace cqde::compos
