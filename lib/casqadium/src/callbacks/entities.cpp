#include <cqde/callbacks/entities.hpp>

#include <cqde/types/EntityManager.hpp>

#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
entityUpdateOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.emplace_or_replace <SubscriberUpdate> (entity);
};

void
entityUpdateOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.removeLater(entity, entityManager.componentType <SubscriberUpdate> ());

};

void
entityUpdateToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberUpdate;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  if ( registry.all_of <SubscriberUpdate> (entity) == true )
    return entityUpdateOff(registry, args);

  entityUpdateOn(registry, args);
};

void
entityInputOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  registry.emplace_or_replace <SubscriberInput> (entity);
};

void
entityInputOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.removeLater(entity, entityManager.componentType <SubscriberInput> ());
};

void
entityInputToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using compos::SubscriberInput;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  if ( registry.all_of <SubscriberInput> (entity) == true )
    return entityInputOff(registry, args);

  entityInputOn(registry, args);
};

} // namespace cqde::callbacks
