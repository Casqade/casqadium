#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/json_helpers.hpp>

#include <json/value.h>

#include <algorithm>


namespace cqde::types
{

const static Json::Value InputBindingAbsoluteJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// InputBindingAbsolute root must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["curve"] = ValueType::realValue;
  reference["curve"].setComment("// 'curve' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  reference["deadzone"] = ValueType::realValue;
  reference["deadzone"].setComment("// 'deadzone' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  return reference;
}();

InputBindingAbsolute::InputBindingAbsolute(
  const InputHwId& inputHwId,
  const Json::Value& json )
  : InputBinding(inputHwId, json)
  , curve{}
  , deadzone{}
{
  try
  {
    jsonValidateObject(json, InputBindingAbsoluteJsonReference);

    curve = json["curve"].asFloat();
    deadzone = json["deadzone"].asFloat();
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(e.what());
  }
}

InputBindingAbsolute::InputBindingAbsolute(
  const InputHwId& _inputId,
  const float _sensitivity )
  : InputBinding(_inputId, _sensitivity)
{}

void
InputBindingAbsolute::handleInput(
  ControlAxis& targetAxis,
  const float amount ) const
{
  // todo : curve + deadzone
  if ( targetAxis.constraint.second >=
       targetAxis.constraint.first )
    targetAxis.value
      = std::clamp(amount * sensitivity,
                   targetAxis.constraint.first,
                   targetAxis.constraint.second );
  else
    targetAxis.value = amount * sensitivity;
}

Json::Value
InputBindingAbsolute::toJson() const
{
  Json::Value json = InputBinding::toJson();

  json["absolute"] = true;
  json["curve"] = curve;
  json["deadzone"] = deadzone;

  return json;
}

} // namespace cqde::types
