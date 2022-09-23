#include <cqde/components/assets/FontAssetUnloadList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value fontAssetUnloadListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& fonts = root["fontsToUnload"];
  fonts = ValueType::arrayValue;
  fonts.setComment("// 'fontsToUnload' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  fonts.append(ValueType::stringValue);
  fonts.begin()->setComment("// 'fontsToUnload' element must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
FontAssetUnloadList::serialize() const
{
  Json::Value json {};

  auto& jsonFonts = json["fontsToUnload"];
  jsonFonts = Json::arrayValue;

  for ( const auto& font : fontsToUnload )
    jsonFonts.append(font.str());

  return json;
}

void
FontAssetUnloadList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, fontAssetUnloadListJsonReference);

  auto& comp = registry.emplace_or_replace <FontAssetUnloadList> (entity);

  for ( const auto& texture : json["fontsToUnload"] )
    comp.fontsToUnload.insert(texture.asString());
}

} // namespace cqde::compos
