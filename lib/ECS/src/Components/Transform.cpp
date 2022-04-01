#include <ECS/Components/Transform.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

Transform::Transform()
  : translation()
  , orientation()
  , scale()
  , modelLocalCache()
{}

glm::mat4
Transform::modelLocal() const
{
  return
    glm::translate(glm::mat4(1.0f), translation) *
    glm::mat4(orientation) *
    glm::scale(glm::mat4(1.0f), scale);
}

void
Transform::serialize( const std::string& json ) const
{}

void
Transform::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <Transform> (entity);
}

void
Transform::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <Transform> ();
  factory.type(entt::hashed_string{"Transform"});
  factory.data <&Transform::translation> ("translation"_hs);
  factory.data <&Transform::orientation> ("orientation"_hs);
  factory.data <&Transform::scale> ("scale"_hs);
  factory.func <&Transform::serialize> ("serialze"_hs);
  factory.func <&Transform::deserialize> ("deserialze"_hs);
}

} // namespace Components

} // namespace ECS
