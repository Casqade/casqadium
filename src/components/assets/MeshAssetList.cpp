#include <cqde/components/assets/MeshAssetList.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value meshAssetListJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& meshes = root["meshes"];
  meshes = ValueType::arrayValue;
  meshes.setComment("// 'meshes' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  meshes.append(ValueType::stringValue);
  meshes.begin()->setComment("// 'meshes' element must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
MeshAssetList::serialize() const
{
  Json::Value json {};

  auto& jsonMeshes = json["meshes"];
  jsonMeshes = Json::arrayValue;

  for ( const auto& mesh : meshes )
    jsonMeshes.append(mesh.str());

  return json;
}

void
MeshAssetList::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  jsonValidateObject(json, meshAssetListJsonReference);

  auto& comp = registry.emplace_or_replace <MeshAssetList> (entity);

  for ( const auto& mesh : json["meshes"] )
    comp.meshes.insert(mesh.asString());
}

} // namespace cqde::compos
