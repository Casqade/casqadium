#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>


namespace cqde
{

bool entity_valid( const entt::entity, const entt::registry& );

std::string component_name( const ComponentType );
bool component_is_empty( const ComponentType );

void each_component(
  const entt::entity,
  const entt::registry&,
  const std::function <bool(const ComponentType)>& );

void each_component(
  const entt::registry&,
  const std::function <bool(const entt::entity,
                            const ComponentType)>& );

void each_componentPool(
  const entt::registry&,
  const std::function <bool(const ComponentType,
                            const entt::registry::base_type&)>& );


template <typename Component>
Component&
component_get(
  entt::registry&     registry,
  const entt::entity  entity )
{
  return registry.template get <Component> (entity);
}

template <typename Component>
const Component&
component_get_const(
  const entt::registry& registry,
  const entt::entity    entity )
{
  return registry.template get <Component> (entity);
}

template <typename Component>
bool
component_exists(
  entt::registry&     registry,
  const entt::entity  entity )
{
  return registry.template all_of <Component> (entity);
}

template <typename Component>
void
component_remove(
  entt::registry&     registry,
  const entt::entity  entity )
{
  registry.template remove <Component> (entity);
}

} // namespace cqde
