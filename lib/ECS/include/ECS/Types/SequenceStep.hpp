#ifndef ECS_TYPES_SEQUENCE_STEP_HPP
#define ECS_TYPES_SEQUENCE_STEP_HPP

#include <ECS/Alias.hpp>

#include <entt/fwd.hpp>


namespace ECS
{

namespace Components
{
struct SequenceManager;
}

namespace Types
{

class SequenceStep
{
public:
  virtual ~SequenceStep() = default;

  virtual bool execute( entt::registry&, const entt::entity, const Components::SequenceManager& );
};

} // namespace Types

} // namespace ECS

#endif
