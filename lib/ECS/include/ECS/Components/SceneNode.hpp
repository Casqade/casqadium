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

  bool dirty;

  SceneNode();

  glm::mat4 modelWorld( entt::registry& );

  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();

private:
  glm::mat4 modelWorldCache;
};

} // namespace Components

} // namespace ECS

#endif
