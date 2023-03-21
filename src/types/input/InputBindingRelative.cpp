#include <cqde/types/input/InputBindingRelative.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <json/value.h>

#include <algorithm>


namespace cqde::types
{

const static Json::Value inputBindingRelativeJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value binding = ValueType::objectValue;
  binding.setComment("// InputBindingRelative root must be a JSON object"s,
                     Json::CommentPlacement::commentBefore);


  binding["absolute"] = ValueType::booleanValue;
  binding["absolute"].setComment("// absolute must be a JSON boolean"s,
                                  Json::CommentPlacement::commentBefore);

  binding["sensitivity"] = ValueType::realValue;
  binding["sensitivity"].setComment("// sensitivity must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  return binding;
}();

InputBindingRelative::InputBindingRelative(
  const InputHwId& inputHwId,
  const Json::Value& json )
  : InputBinding(inputHwId, json)
{}

InputBindingRelative::InputBindingRelative(
  const InputHwId& _inputId,
  const float _sensitivity )
  : InputBinding(_inputId, _sensitivity)
{}

void
InputBindingRelative::handleInput(
  ControlAxis& targetAxis,
  const float amount ) const
{
  if ( targetAxis.constraint.second >=
       targetAxis.constraint.first )
    targetAxis.value
      = std::clamp(targetAxis.value + amount * sensitivity,
                   targetAxis.constraint.first,
                   targetAxis.constraint.second );
  else
    targetAxis.value += amount * sensitivity;
}

Json::Value
InputBindingRelative::toJson() const
{
  Json::Value json = InputBinding::toJson();

  json["absolute"] = false;

  return json;
}

} // namespace cqde::types
