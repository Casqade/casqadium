#ifndef ECS_COMPONENTS_TRANSFORM_HPP
#define ECS_COMPONENTS_TRANSFORM_HPP

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>


namespace ECS
{

namespace Components
{

struct SceneNode;


struct Transform
{
  glm::vec3 translation;
  glm::quat orientation;
  glm::vec3 scale;
  glm::vec3 scaleWorld;


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

} // namespace Components

} // namespace ECS

#endif
