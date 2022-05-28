#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::types
{

InputBindingAbsolute::InputBindingAbsolute(
  const cqde::InputHwId& _inputId,
  const float _sensitivity )
  : InputBinding(_inputId, _sensitivity)
{}

void
InputBindingAbsolute::handleInput(
  cqde::types::ControlAxis& targetAxis,
  const float amount ) const
{
  // todo : curve + deadzone
  targetAxis.value
    = std::clamp(amount * sensitivity,
                 targetAxis.constraint.first,
                 targetAxis.constraint.second );
}

} // namespace cqde::types
