#include <cqde/components/assets/FontAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value fontAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& fonts = root["fonts"];
  fonts = ValueType::arrayValue;
  fonts.setComment("// 'fonts' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  fonts.append(ValueType::stringValue);
  fonts.begin()->setComment("// 'fonts' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
FontAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonFonts = json["fonts"];
  jsonFonts = Json::arrayValue;

  for ( const auto& font : fonts )
    jsonFonts.append(font.str());

  return json;
}

void
FontAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, fontAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <FontAssetList> (entity);

  for ( const auto& font : json["fonts"] )
    comp.fonts.insert(font.asString());
}

} // namespace cqde::compos
