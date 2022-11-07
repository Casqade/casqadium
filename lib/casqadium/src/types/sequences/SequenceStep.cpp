#include <cqde/types/sequences/SequenceStep.hpp>

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
  entt::registry& registry,
  const entt::entity entity )
{}

bool
SequenceStep::execute(
  entt::registry& registry,
  const entt::entity entity )
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

} // namespace cqde::types
