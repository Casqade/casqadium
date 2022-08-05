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
  reference.setComment("// JSON root must be an object"s,
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
  const cqde::InputHwId& inputHwId,
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
