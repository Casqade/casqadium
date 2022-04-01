#ifndef ECS_COMPONENTS_SCENENODE_HPP
#define ECS_COMPONENTS_SCENENODE_HPP

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <string>

#include <ECS/Components/Tag.hpp>


namespace ECS
{

namespace Components
{

struct SceneNode
{
  EntityReference parent;

  glm::vec3 scaleWorld;

  glm::mat4 modelWorld( entt::registry& ) const;

  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
