#pragma once

#include <cqde/types/SequenceStep.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <memory>
#include <set>


namespace cqde::compos
{

struct SequenceManager
{
  std::set <std::shared_ptr <types::SequenceStep>> steps;

  std::set <std::shared_ptr <types::SequenceStep>>::iterator currentStep;


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
