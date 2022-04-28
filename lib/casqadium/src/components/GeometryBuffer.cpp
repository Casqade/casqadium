#include <cqde/components/GeometryBuffer.hpp>

#include <entt/entt.hpp>


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
GeometryBuffer::serialize( const std::string& json ) const
{}

void
GeometryBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <GeometryBuffer> (entity);
}

void
GeometryBuffer::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <GeometryBuffer> ();
  factory.type(entt::hashed_string{"GeometryBuffer"});
  factory.func <&GeometryBuffer::serialize> ("serialze"_hs);
  factory.func <&GeometryBuffer::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
