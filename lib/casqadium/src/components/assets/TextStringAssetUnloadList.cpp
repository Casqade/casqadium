#include <cqde/components/assets/TextStringAssetUnloadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value textStringAssetUnloadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& textStrings = root["textToUnload"];
  textStrings = ValueType::arrayValue;
  textStrings.setComment("// 'textToUnload' must be a JSON array"s,
                          Json::CommentPlacement::commentBefore);

  textStrings.append(ValueType::stringValue);
  textStrings.begin()->setComment("// 'textToUnload' element must be a JSON string"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
TextStringAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonTextStrings = json["textToUnload"];
  jsonTextStrings = Json::arrayValue;

  for ( const auto& text : textToUnload )
    jsonTextStrings.append(text.str());

  return json;
}

void
TextStringAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, textStringAssetUnloadListJsonReference);

  auto& comp = registry.emplace_or_replace <TextStringAssetUnloadList> (entity);

  for ( const auto& text : json["textToUnload"] )
    comp.textToUnload.insert(text.asString());
}

} // namespace cqde::compos
