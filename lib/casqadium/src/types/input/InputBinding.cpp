#include <cqde/types/input/InputBinding.hpp>


namespace cqde::types
{

InputBinding::InputBinding(
  const cqde::InputHwId& _inputId,
  const float _sensitivity )
  : inputId(_inputId)
  , sensitivity(_sensitivity)
{}

void
InputBinding::handleInput(
  cqde::types::ControlAxis&,
  const float amount ) const
{}

} // namespace cqde::types
