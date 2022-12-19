#include <cqde/callbacks/common_routine.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/EntityList.hpp>
#include <cqde/components/SystemList.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
interact(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::CallbackManager;
  using compos::InteractionSource;
  using compos::InteractionListener;

  const auto eInteractionSource = std::any_cast <entt::entity> (args.at(0));
  const auto eListener = findInteractionTarget(registry, eInteractionSource);

  if ( eListener == entt::null )
    return;

  const auto& cListener = registry.get <InteractionListener> (eListener);

  auto& callbackManager = registry.ctx().at <CallbackManager> ();

  for ( const auto& callback : cListener.callbacks )
    callbackManager.execute(callback, registry, {eListener, eInteractionSource});
}


void
entityRemove(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  entityManager.removeLater(entity);
}

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
entitiesUpdateOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberUpdate;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto& entityManager = registry.ctx().at <const EntityManager> ();

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityManager.get_if_valid(entityId, registry);

    if ( entity != entt::null )
      registry.emplace_or_replace <SubscriberUpdate> (entity);
  }
}

void
entitiesUpdateOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberUpdate;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  auto& entityList = registry.get <const EntityList> (entity);

  const auto componentType = entityManager.componentType <SubscriberUpdate> ();

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityManager.get_if_valid(entityId, registry);

    if ( entity != entt::null )
      entityManager.removeLater(entity, componentType);
  }
}

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

void
entitiesInputOn(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberInput;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));
  auto& entityManager = registry.ctx().at <const EntityManager> ();

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityManager.get_if_valid(entityId, registry);

    if ( entity != entt::null )
      registry.emplace_or_replace <SubscriberInput> (entity);
  }
}

void
entitiesInputOff(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberInput;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().at <EntityManager> ();
  auto& entityList = registry.get <const EntityList> (entity);

  const auto componentType = entityManager.componentType <SubscriberInput> ();

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityManager.get_if_valid(entityId, registry);

    if ( entity != entt::null )
      entityManager.removeLater(entity, componentType);
  }
}


void
systemsActivate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::SystemManager;
  using compos::SystemList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& systemManager = registry.ctx().at <SystemManager> ();
  auto& systemList = registry.get <const SystemList> (entity);

  for ( const auto& systemId : systemList.systems )
    systemManager.activate(systemId);
}

void
systemsDeactivate(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::SystemManager;
  using compos::SystemList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& systemManager = registry.ctx().at <SystemManager> ();
  auto& systemList = registry.get <const SystemList> (entity);

  for ( const auto& systemId : systemList.systems )
    systemManager.deactivate(systemId);
}


void
quickSave(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::SnapshotManager;

  SnapshotManager::Write(registry, "quick.json");
};

void
quickLoad(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::CallbackManager;
  using types::SnapshotManager;
  using types::UserManager;

  registry.ctx().at <CallbackManager> ().executeLater(
  [] (  entt::registry& registry ,
        const CallbackManager::CallbackArgs& )
  {
    try
    {
      const auto snapshotPath = registry.ctx().at <UserManager> ().snapshotsRoot();
      SnapshotManager::Load(registry, snapshotPath / "quick.json");
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR(e.what());
    }
  });
};

} // namespace cqde::callbacks
