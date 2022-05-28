#include <cqde/types/input/InputBindingRelative.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::types
{

InputBindingRelative::InputBindingRelative(
  const cqde::InputHwId& _inputId,
  const float _sensitivity )
  : InputBinding(_inputId, _sensitivity)
{}

void
InputBindingRelative::handleInput(
  cqde::types::ControlAxis& targetAxis,
  const float amount ) const
{
  targetAxis.value
    = std::clamp(targetAxis.value + amount * sensitivity,
                 targetAxis.constraint.first,
                 targetAxis.constraint.second );
}

} // namespace cqde::types
