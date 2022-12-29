#include <cqde/components/assets/TextStringAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textStringAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textStrings = root["text"];
  textStrings = ValueType::arrayValue;
  textStrings.setComment("// 'text' must be a JSON array"s,
                          Json::CommentPlacement::commentBefore);

  textStrings.append(ValueType::stringValue);
  textStrings.begin()->setComment("// 'text' element must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextStringAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonTextStrings = json["text"];
  jsonTextStrings = Json::arrayValue;

  for ( const auto& text : text )
    jsonTextStrings.append(text.str());

  return json;
}

void
TextStringAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, textStringAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <TextStringAssetList> (entity);

  for ( const auto& text : json["text"] )
    comp.text.insert(text.asString());
}

} // namespace cqde::compos
