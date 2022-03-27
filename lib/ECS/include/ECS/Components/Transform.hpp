#ifndef ECS_COMPONENTS_TRANSFORM_HPP
#define ECS_COMPONENTS_TRANSFORM_HPP

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>


namespace ECS
{

namespace Components
{

struct Transform
{
  glm::vec3 translation;
  glm::quat orientation;
  glm::vec3 scale;

  bool dirty;

  Transform();

  glm::mat4 modelLocal();

  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();

private:
  glm::mat4 modelLocalCache;
};

} // namespace Components

} // namespace ECS

#endif
