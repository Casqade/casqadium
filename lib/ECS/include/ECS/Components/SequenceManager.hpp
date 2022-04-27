#pragma once

#include <ECS/Types/SequenceStep.hpp>

#include <entt/fwd.hpp>

#include <memory>
#include <set>


namespace ECS
{

namespace Components
{

struct SequenceManager
{
  std::set <std::shared_ptr <ECS::Types::SequenceStep>> steps;

  std::set <std::shared_ptr <ECS::Types::SequenceStep>>::iterator currentStep;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS
