#ifndef ECS_COMPONENTS_INPUT_CONTROLLER_HPP
#define ECS_COMPONENTS_INPUT_CONTROLLER_HPP

#include <ECS/Alias.hpp>
#include <ECS/Types/InputAxis.hpp>

#include <entt/fwd.hpp>

#include <map>
#include <string>


namespace ECS
{

namespace Components
{

struct InputController
{
  std::map <InputId, Types::InputAxis> inputs;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
