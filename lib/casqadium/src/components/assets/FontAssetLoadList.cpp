#include <cqde/components/assets/FontAssetLoadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value fontAssetLoadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& fonts = root["fontsToLoad"];
  fonts = ValueType::arrayValue;
  fonts.setComment("// 'fontsToLoad' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  fonts.append(ValueType::stringValue);
  fonts.begin()->setComment("// 'fontsToLoad' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
FontAssetLoadList::serialize() const
{
  Json::Value json {};

  auto& jsonFonts = json["fontsToLoad"];
  jsonFonts = Json::arrayValue;

  for ( const auto& font : fontsToLoad )
    jsonFonts.append(font.str());

  return json;
}

void
FontAssetLoadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, fontAssetLoadListJsonReference);

  auto& comp = registry.emplace_or_replace <FontAssetLoadList> (entity);

  for ( const auto& texture : json["fontsToLoad"] )
    comp.fontsToLoad.insert(texture.asString());
}

} // namespace cqde::compos
