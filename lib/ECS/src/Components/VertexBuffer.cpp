#include <ECS/Components/VertexBuffer.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

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
  factory.type(entt::hashed_string{"VertexBuffer"});
  factory.func <&GeometryBuffer::serialize> ("serialze"_hs);
  factory.func <&GeometryBuffer::deserialize> ("deserialize"_hs);
}


bool
VertexBuffer::operator > ( const VertexBuffer& other ) const
{
  return depth > other.depth;
}

} // namespace Components

} // namespace ECS
