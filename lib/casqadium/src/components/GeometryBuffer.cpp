#include <cqde/components/GeometryBuffer.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

GeometryBuffer::GeometryBuffer( const std::vector <glm::vec3>& _vertexes )
  : vertexes(_vertexes)
{}

GeometryBuffer::GeometryBuffer( const std::array <glm::vec3, 4>& _vertexes )
{
  vertexes = std::vector <glm::vec3> ( _vertexes.begin(), _vertexes.end() );
}

void
GeometryBuffer::serialize( Json::Value& json ) const
{}

void
GeometryBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <GeometryBuffer> (entity);
}

void
GeometryBuffer::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <GeometryBuffer> ();
  factory.type("GeometryBuffer"_hs);
  factory.func <&GeometryBuffer::serialize> ("serialize"_hs);
  factory.func <&GeometryBuffer::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
