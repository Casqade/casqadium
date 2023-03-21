#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/common.hpp>

#include <cqde/json_helpers.hpp>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value InputBindingJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// InputBinding root must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  reference["absolute"] = ValueType::booleanValue;
  reference["absolute"].setComment("// 'absolute' must be a JSON boolean"s,
                                    Json::CommentPlacement::commentBefore);

  reference["sensitivity"] = ValueType::realValue;
  reference["sensitivity"].setComment("// 'sensitivity' must be a JSON float"s,
                                      Json::CommentPlacement::commentBefore);

  return reference;
}();


InputBinding::InputBinding(
  const InputHwId& inputHwId,
  const Json::Value& json )
  : inputId{inputHwId}
  , sensitivity{}
{
  try
  {
    jsonValidateObject(json, InputBindingJsonReference);

    sensitivity = json["sensitivity"].asFloat();
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(e.what());
  }
}

InputBinding::InputBinding(
  const InputHwId& inputHwId,
  const float _sensitivity )
  : inputId(inputHwId)
  , sensitivity(_sensitivity)
{}

void
InputBinding::handleInput(
  ControlAxis&,
  const float amount ) const
{
  CQDE_ASSERT_DEBUG(false, return;);
}

Json::Value
InputBinding::toJson() const
{
  Json::Value json {};

  json["sensitivity"] = sensitivity;

  return json;
}

std::shared_ptr <InputBinding>
InputBinding::FromJson( const InputHwId& inputHwId, const Json::Value& json )
{
  try
  {
    jsonValidateObject(json, InputBindingJsonReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(e.what());
  }

  std::shared_ptr <InputBinding> result {};

  if ( json["absolute"].asBool() == true )
    result = std::make_shared <InputBindingAbsolute> (inputHwId, json);
  else
    result = std::make_shared <InputBindingRelative> (inputHwId, json);

  return result;
}

bool
InputBindingComparator::operator () ( const std::shared_ptr <InputBinding>& lhs, const std::shared_ptr <InputBinding>& rhs ) const
{
  return lhs->inputId < rhs->inputId;
}

bool
InputBindingComparator::operator () ( const InputHwId& lhs, const std::shared_ptr <InputBinding>& rhs ) const
{
  return lhs < rhs->inputId;
}

bool
InputBindingComparator::operator () ( const std::shared_ptr <InputBinding>& lhs, const InputHwId& rhs ) const
{
  return lhs->inputId < rhs;
}

} // namespace cqde::types
