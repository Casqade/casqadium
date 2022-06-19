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


bool
InputBindingComparator::operator () ( const std::shared_ptr <InputBinding>& lhs, const std::shared_ptr <InputBinding>& rhs ) const
{
  return lhs->inputId < rhs->inputId;
}

bool
InputBindingComparator::operator () ( const cqde::InputHwId& lhs, const std::shared_ptr <InputBinding>& rhs ) const
{
  return lhs < rhs->inputId;
}

bool
InputBindingComparator::operator () ( const std::shared_ptr <InputBinding>& lhs, const cqde::InputHwId& rhs ) const
{
  return lhs->inputId < rhs;
}

} // namespace cqde::types
