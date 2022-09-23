#include <cqde/components/assets/TextStringAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textStringAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textStrings = root["textToLoad"];
  textStrings = ValueType::arrayValue;
  textStrings.setComment("// 'textToLoad' must be a JSON array"s,
                          Json::CommentPlacement::commentBefore);

  textStrings.append(ValueType::stringValue);
  textStrings.begin()->setComment("// 'textToLoad' element must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextStringAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonTextStrings = json["textToLoad"];
  jsonTextStrings = Json::arrayValue;

  for ( const auto& text : textToLoad )
    jsonTextStrings.append(text.str());

  return json;
}

void
TextStringAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, textStringAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <TextStringAssetLoadList> (entity);

  for ( const auto& text : json["textToLoad"] )
    comp.textToLoad.insert(text.asString());
}

} // namespace cqde::compos
