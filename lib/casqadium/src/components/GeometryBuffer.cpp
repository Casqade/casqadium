#include <cqde/components/GeometryBuffer.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value geometryBufferJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  root["buffer"] = ValueType::stringValue;
  root["buffer"].setComment("// 'buffer' must be a JSON string"s,
                            Json::CommentPlacement::commentBefore);

  return root;
}();

GeometryBuffer::GeometryBuffer( const std::vector <glm::vec3>& _vertexes )
  : vertexes(_vertexes)
{}

GeometryBuffer::GeometryBuffer( const std::array <glm::vec3, 4>& _vertexes )
{
  vertexes = std::vector <glm::vec3> ( _vertexes.begin(), _vertexes.end() );
}

Json::Value
GeometryBuffer::serialize() const
{
  Json::Value json {};
  json["buffer"] = buffer.str();

  return json;
}

void
GeometryBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json )
{
  jsonValidateObject(json, geometryBufferJsonReference);

  auto& comp = registry.emplace <GeometryBuffer> (entity);

  comp.buffer = json["buffer"].asString();
}

} // namespace cqde::compos
