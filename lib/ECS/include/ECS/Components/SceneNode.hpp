#ifndef ECS_COMPONENTS_SCENENODE_HPP
#define ECS_COMPONENTS_SCENENODE_HPP

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <set>
#include <string>

#include <ECS/Components/Tag.hpp>


namespace ECS
{

namespace Components
{

struct SceneNode
{
  EntityReference parent;
  std::set <EntityReference> children;


  SceneNode();

  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
