#include <cqde/components/InteractionSource.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

std::string
InteractionSource::InteractionTypeToString(
  const InteractionSource::Type type )
{
  using InteractionType = InteractionSource::Type;

  switch (type)
  {
    case InteractionType::PhysicsProbe:
      return "physicsProbe";

    case InteractionType::ViewportCenter:
      return "viewportCenter";

    case InteractionType::MousePos:
      return "mousePos";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

InteractionSource::Type
InteractionSource::InteractionTypeFromString(
  const std::string& type )
{
  using fmt::format;
  using InteractionType = InteractionSource::Type;

  if ( type == InteractionTypeToString(InteractionType::PhysicsProbe) )
    return InteractionType::PhysicsProbe;

  if ( type == InteractionTypeToString(InteractionType::ViewportCenter) )
    return InteractionType::ViewportCenter;

  if ( type == InteractionTypeToString(InteractionType::MousePos) )
    return InteractionType::MousePos;

  throw std::runtime_error(
    format("'{}' is not a valid interaction type", type));
}

const static Json::Value interactionSourceJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& radius = root["radius"];
  radius = ValueType::realValue;
  radius.setComment("// 'radius' must be a JSON float"s,
                    Json::CommentPlacement::commentBefore);

  auto& type = root["type"];
  type = ValueType::stringValue;
  type.setComment("// 'type' must be a JSON string"s,
                  Json::CommentPlacement::commentBefore);

  auto& actions = root["actions"];
  actions = ValueType::arrayValue;
  actions.setComment("// 'actions' entry must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  actions.append(ValueType::stringValue);
  actions.begin()->setComment("// 'actions' entry element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
InteractionSource::serialize() const
{
  Json::Value json {};

  json["radius"] = radius;
  json["type"] = InteractionTypeToString(type);

  auto& jsonActions = json["actions"];
  jsonActions = Json::arrayValue;

  for ( const auto& actionId : actions )
    jsonActions.append(actionId.str());

  return json;
}

void
InteractionSource::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, interactionSourceJsonReference);

  auto& comp = registry.emplace_or_replace <InteractionSource> (entity);

  comp.radius = json["radius"].asFloat();
  comp.type = InteractionTypeFromString(json["type"].asString());

  for ( const auto& actionId : json["actions"] )
    comp.actions.insert(actionId.asString());
}

} // namespace cqde::compos
