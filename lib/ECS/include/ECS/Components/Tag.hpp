#ifndef ECS_COMPONENTS_TAG_HPP
#define ECS_COMPONENTS_TAG_HPP

#include <entt/fwd.hpp>
#include <entt/core/hashed_string.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <map>


namespace ECS
{

namespace Components
{

struct Tag
{
  entt::hashed_string id;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();

  static Tag Generate( entt::hashed_string = {} );
};

struct EntityReference
{
  entt::hashed_string id;

  EntityReference();

  entt::entity get() const;
};

} // namespace Components

} // namespace ECS

#endif
