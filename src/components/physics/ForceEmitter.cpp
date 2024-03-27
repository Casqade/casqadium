#include <cqde/components/physics/ForceEmitter.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec4.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value forceEmitterJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& useWorldSpace = root["useWorldSpace"];
  useWorldSpace = ValueType::booleanValue;
  useWorldSpace.setComment("// 'useWorldSpace' must be a JSON boolean"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
ForceEmitter::serialize() const
{
  Json::Value json {};

  json["force"] << force;
  json["useWorldSpace"] = useWorldSpace;

  return json;
}

void
ForceEmitter::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const IdMap& idMap )
{
  using fmt::format;
  jsonValidateObject(json, forceEmitterJsonReference);

  auto& comp = registry.emplace_or_replace <ForceEmitter> (entity);

  comp.useWorldSpace = json["useWorldSpace"].asBool();

  try
  {
    comp.force << json["force"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'force' parse error: {}",
              e.what()));
  }
}

} // namespace cqde::compos
