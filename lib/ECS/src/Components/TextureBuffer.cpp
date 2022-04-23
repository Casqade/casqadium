#include <ECS/Components/TextureBuffer.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

TextureBuffer::TextureBuffer()
  : textures(2, TextureId("null"))  // front & back faces
{}

void
TextureBuffer::serialize( const std::string& json ) const
{}

void
TextureBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <TextureBuffer> (entity);
}

void
TextureBuffer::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <TextureBuffer> ();
  factory.type(entt::hashed_string{"TextureBuffer"});
  factory.data <&TextureBuffer::textures> ("TextureBuffer"_hs);
  factory.func <&TextureBuffer::serialize> ("serialze"_hs);
  factory.func <&TextureBuffer::deserialize> ("deserialze"_hs);
}

} // namespace Components

} // namespace ECS
