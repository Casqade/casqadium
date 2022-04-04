#include <ECS/Components/Transform.hpp>
#include <ECS/Components/SceneNode.hpp>

#include <entt/entt.hpp>

#include <glm/mat4x4.hpp>


namespace ECS
{

namespace Components
{

glm::mat4
Transform::modelLocal() const
{
  return
    glm::translate(glm::mat4(1.0f), translation) *
    glm::toMat4(orientation) *
    glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3
Transform::front() const
{
  return glm::rotate( orientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Transform::right() const
{
  return glm::rotate(  orientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Transform::up() const
{
  return glm::rotate( orientation, {0.0f, 1.0f, 0.0f} );
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
  factory.data <&Transform::scaleWorld> ("scaleWorld"_hs);
  factory.func <&Transform::modelLocal> ("modelLocal"_hs);
  factory.func <&Transform::front> ("front"_hs);
  factory.func <&Transform::right> ("right"_hs);
  factory.func <&Transform::up> ("up"_hs);
  factory.func <&Transform::serialize> ("serialze"_hs);
  factory.func <&Transform::deserialize> ("deserialze"_hs);
}

} // namespace Components

} // namespace ECS
