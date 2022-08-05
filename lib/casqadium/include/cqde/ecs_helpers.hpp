#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>


namespace cqde
{

std::string component_name( const ComponentType );

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

} // namespace cqde
