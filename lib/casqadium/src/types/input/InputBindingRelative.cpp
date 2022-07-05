#include <cqde/types/input/InputBindingRelative.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value inputBindingJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value binding = ValueType::objectValue;
  binding.setComment("// input binding value root must be an object"s,
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
  const cqde::InputHwId& inputHwId,
  const Json::Value& json )
  : InputBinding(inputHwId, json)
{}

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

Json::Value
InputBindingRelative::toJson() const
{
  return InputBinding::toJson();
}

} // namespace cqde::types
