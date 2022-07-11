#include <cqde/types/input/ControlAxis.hpp>

#include <cqde/common.hpp>

#include <cqde/json_helpers.hpp>

#include <entt/entt.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

const static Json::Value controlAxisJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                   Json::CommentPlacement::commentBefore);

  root["value"] = ValueType::realValue;
  root["value"].setComment("// 'value' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);

  root["constraint"] = ValueType::arrayValue;
  root["constraint"].setComment("// 'constraint' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  root["constraint"].append(ValueType::realValue);
  root["constraint"].begin()->setComment("// 'constraint' element must be a JSON float"s,
                                          Json::CommentPlacement::commentBefore);

  root["callbacks"] = ValueType::arrayValue;
  root["callbacks"].setComment("// 'callbacks' must be a JSON array"s,
                                Json::CommentPlacement::commentBefore);

  root["callbacks"].append(ValueType::stringValue);
  root["callbacks"].begin()->setComment("// 'callbacks' element must be a JSON string"s,
                                        Json::CommentPlacement::commentBefore);

  return root;
}();

ControlAxis::ControlAxis( const Json::Value& json )
{
  using fmt::format;

  jsonValidateObject(json, controlAxisJsonReference);

  if ( json["constraint"].size() != 2 )
    throw std::runtime_error(
      format("'constraint' array size must be 2 elements"));

  value = json["value"].asFloat();

  constraint.first = json["constraint"][0].asFloat();
  constraint.second = json["constraint"][1].asFloat();

  for ( const auto& callback : json["callbacks"] )
    callbacks.insert(callback.asString());
}

bool
ControlAxis::operator == ( const ControlAxis& other ) const
{
  return  std::tie(value, constraint, callbacks) ==
          std::tie(other.value, other.constraint, other.callbacks);
}

Json::Value
ControlAxis::toJson() const
{
  Json::Value json {};

  json["value"] = value;
  json["constraint"].append(constraint.first);
  json["constraint"].append(constraint.second);

  for ( const auto& callback : callbacks )
    json["callbacks"] = callback.str();

  return json;
}

} // namespace cqde::types
