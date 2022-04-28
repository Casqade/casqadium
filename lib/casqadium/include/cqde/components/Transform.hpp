#pragma once

#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>


namespace cqde
{

namespace compos
{

struct Transform
{
  glm::vec3 translation;
  glm::quat orientation;
  glm::vec3 scale;
  glm::vec3 scaleWorld;


  Transform(  const glm::vec3& origin = {},
              const glm::quat& orientation = glm::vec3{},
              const glm::vec3& scale = glm::vec3(1.0f),
              const glm::vec3& scaleWorld = glm::vec3(1.0f) );

  glm::mat4 modelLocal() const;

  glm::vec3 front() const;
  glm::vec3 right() const;
  glm::vec3 up() const;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace compos


glm::vec3 ToLocalSpace( const glm::vec3& point,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::quat ToLocalSpace( const glm::quat& orientation,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::mat4 ToLocalSpace( const glm::mat4& matrix,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::vec3 ToWorldSpace( const glm::vec3& point,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::quat ToWorldSpace( const glm::quat& orientation,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::mat4 ToWorldSpace( const glm::mat4& matrix,
                        entt::registry&,
                        const compos::Transform&,
                        const compos::SceneNode& );

glm::mat4 GetWorldMatrix( entt::registry&,
                          const compos::Transform&,
                          const compos::SceneNode& );

} // namespace cqde
