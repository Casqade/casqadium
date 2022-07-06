#include <cqde/components/SequenceManager.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
SequenceManager::serialize() const
{}

void
SequenceManager::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <SequenceManager> (entity);
}

} // namespace cqde::compos
