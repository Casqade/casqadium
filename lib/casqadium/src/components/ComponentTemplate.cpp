#include <cqde/components/ComponentTemplate.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
ComponentTemplate::serialize() const
{}

void
ComponentTemplate::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <ComponentTemplate> (entity);
}

} // namespace cqde::compos
