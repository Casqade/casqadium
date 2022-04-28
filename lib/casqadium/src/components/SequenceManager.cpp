#include <cqde/components/SequenceManager.hpp>

#include <entt/entt.hpp>


namespace cqde::compos
{

void
SequenceManager::serialize( const std::string& json ) const
{}

void
SequenceManager::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <SequenceManager> (entity);
}

void
SequenceManager::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <SequenceManager> ();
  factory.type(entt::hashed_string{"SequenceManager"});
  factory.func <&SequenceManager::serialize> ("serialze"_hs);
  factory.func <&SequenceManager::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
