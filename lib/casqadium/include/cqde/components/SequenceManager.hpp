#pragma once

#include <cqde/types/SequenceStep.hpp>

#include <entt/fwd.hpp>

#include <memory>
#include <set>


namespace cqde::compos
{

struct SequenceManager
{
  std::set <std::shared_ptr <types::SequenceStep>> steps;

  std::set <std::shared_ptr <types::SequenceStep>>::iterator currentStep;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace cqde::compos
