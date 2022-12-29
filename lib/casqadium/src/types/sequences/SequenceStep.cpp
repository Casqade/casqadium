#include <cqde/types/sequences/SequenceStep.hpp>

#include <cqde/json_helpers.hpp>

#include <json/value.h>


namespace cqde::types
{

std::string
SequenceStep::name() const
{
  return "SequenceStep";
}

void
SequenceStep::init(
  entt::registry&,
  const entt::entity )
{}

bool
SequenceStep::execute(
  entt::registry&,
  const entt::entity )
{
  return false;
}

void
SequenceStep::ui_show(
  const entt::registry&,
  const entt::entity )
{}

Json::Value
SequenceStep::toJson() const
{
  return Json::objectValue;
}

void
SequenceStep::fromJson(
  const Json::Value& )
{}


const static Json::Value sequenceInitializationStatusJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// SequenceInitializationStatus root must be a JSON object"s,
                   Json::CommentPlacement::commentBefore);

  auto& initialized = root["initialized"];
  initialized = ValueType::booleanValue;
  initialized.setComment("// 'initialized' must be a JSON boolean"s,
                          Json::CommentPlacement::commentBefore);

  return root;
}();

void
SequenceInitializationStatus::init()
{
  mInitialized = true;
}

bool
SequenceInitializationStatus::initialized() const
{
  return mInitialized;
}

Json::Value
SequenceInitializationStatus::toJson() const
{
  Json::Value json {};

  json["initialized"] = mInitialized;

  return json;
}

void
SequenceInitializationStatus::fromJson(
  const Json::Value& json )
{
  jsonValidateObject(json, sequenceInitializationStatusJsonReference);

  mInitialized = json["initialized"].asBool();
}

} // namespace cqde::types
