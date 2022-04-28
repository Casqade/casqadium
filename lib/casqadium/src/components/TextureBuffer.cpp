#include <cqde/components/TextureBuffer.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

TextureBuffer::TextureBuffer()
  : textures(2, TextureId("null"))  // front & back faces
{}

void
TextureBuffer::serialize( Json::Value& json ) const
{}

void
TextureBuffer::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <TextureBuffer> (entity);
}

void
TextureBuffer::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <TextureBuffer> ();
  factory.type("TextureBuffer"_hs);
  factory.data <&TextureBuffer::textures> ("TextureBuffer"_hs);
  factory.func <&TextureBuffer::serialize> ("serialize"_hs);
  factory.func <&TextureBuffer::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
