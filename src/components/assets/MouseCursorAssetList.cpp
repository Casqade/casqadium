#include <cqde/components/assets/MouseCursorAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value mouseCursorAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& cursors = root["cursors"];
  cursors = ValueType::arrayValue;
  cursors.setComment("// 'cursors' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  cursors.append(ValueType::stringValue);
  cursors.begin()->setComment("// 'cursors' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
MouseCursorAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonCursors = json["cursors"];
  jsonCursors = Json::arrayValue;

  for ( const auto& cursor : cursors )
    jsonCursors.append(cursor.str());

  return json;
}

void
MouseCursorAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, mouseCursorAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <MouseCursorAssetList> (entity);

  for ( const auto& cursor : json["cursors"] )
    comp.cursors.insert(cursor.asString());
}

} // namespace cqde::compos
