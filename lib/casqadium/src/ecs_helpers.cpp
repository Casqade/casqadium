#include <cqde/ecs_helpers.hpp>

#include <entt/meta/resolve.hpp>


namespace cqde
{

std::string
component_name(
  const ComponentType component )
{
  using namespace entt::literals;

  return entt::resolve(component)
    .prop("typename"_hs)
    .value()
    .cast <std::string> ();
}

bool
component_is_empty(
  const ComponentType component )
{
  using namespace entt::literals;

  return entt::resolve(component)
    .prop("empty"_hs)
    .value()
    .cast <bool> ();
}

void
each_component(
  const entt::entity entity,
  const entt::registry& registry,
  const std::function <bool(const ComponentType)>& func )
{
  for ( const auto [componentType, entities] : registry.storage() )
    if ( const auto iter = entities.find(entity); iter != entities.end() )
      if ( func(componentType) == false )
        return;
}

void
each_component(
  const entt::registry& registry,
  const std::function <bool(const entt::entity,
                            const ComponentType)>& func )
{
  for ( auto [componentType, entities] : registry.storage() )
    for ( auto entity : entities )
      if ( func(entity, componentType) == false )
        return;
}

void
each_componentPool(
  const entt::registry& registry,
  const std::function <bool(const ComponentType,
                            const entt::registry::base_type&)>& func )
{
  for ( auto [componentType, entities] : registry.storage() )
    if ( func(componentType, entities) == false )
      return;
}

} // namespace cqde
