#include <cqde/ecs_helpers.hpp>

#include <entt/meta/resolve.hpp>


namespace cqde
{

std::string
component_name( const ComponentType component )
{
  using namespace entt::literals;

  return entt::resolve(component)
    .prop("typename"_hs)
    .value()
    .cast <std::string> ();
}

void
each_component(
  const entt::entity entity,
  const entt::registry& registry,
  const std::function <void(const ComponentType)>& func )
{
  for ( const auto [componentType, entities] : registry.storage() )
    if ( const auto iter = entities.find(entity); iter != entities.end() )
      func(componentType);
}

void
each_component(
  const entt::registry& registry,
  const std::function <void(const entt::entity,
                            const ComponentType)>& func )
{
  for ( auto [componentType, entities] : registry.storage() )
    for ( auto entity : entities )
      func(entity, componentType);
}

void
each_componentPool(
  const entt::registry& registry,
  const std::function <void(const ComponentType,
                            const entt::registry::base_type&)>& func )
{
  for ( auto [componentType, entities] : registry.storage() )
    func(componentType, entities);
}

} // namespace cqde
