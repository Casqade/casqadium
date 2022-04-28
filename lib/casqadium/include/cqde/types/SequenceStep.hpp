#pragma once

#include <cqde/alias.hpp>
#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class SequenceStep
{
public:
  virtual ~SequenceStep() = default;

  virtual bool execute( entt::registry&, const entt::entity, const compos::SequenceManager& );
};

} // namespace cqde::types
