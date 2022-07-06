#pragma once

#include <cqde/alias.hpp>

#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/resolve.hpp>


namespace cqde
{

std::string component_name( const ComponentType );

void each_component(
  const entt::entity,
  const entt::registry&,
  const std::function <void(ComponentType)>& );

void each_component(
  const entt::registry&,
  const std::function <void(entt::entity,
                            ComponentType)>& );

void each_componentPool(
  const entt::registry&,
  const std::function <void(const ComponentType,
                            const entt::registry::base_type&)>& );


template <typename Component>
Component&
component_get(
  entt::registry& registry,
  entt::entity entity )
{
  return registry.template get <Component> (entity);
}

template <typename Component>
decltype(auto)
component_register(
  const std::string& name )
{
  using namespace entt::literals;

  auto factory = entt::meta <Component> ();
  factory.type(entt::hashed_string(name.c_str()));
  factory.prop("typename"_hs, name);
  factory.template func <&component_get <Component>, entt::as_ref_t> ("get"_hs);
  factory.template func <&Component::serialize> ("serialize"_hs);
  factory.template func <&Component::deserialize> ("deserialize"_hs);

  return factory;
}

} // namespace cqde
