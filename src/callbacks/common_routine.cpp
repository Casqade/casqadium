#include <cqde/callbacks/common_routine.hpp>

#include <cqde/common.hpp>
#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/logger.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/InteractionProbe.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/InteractionSourceActiveAction.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/EntityList.hpp>
#include <cqde/components/SystemList.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>


namespace cqde::callbacks
{

void
interact(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::CallbackManager;
  using compos::InteractionSource;
  using compos::InteractionSourceActiveAction;
  using compos::InteractionListener;

  const auto eSource = std::any_cast <entt::entity> (args.at(0));

  auto&& [cSource, cSourceAction]
    = registry.get <const InteractionSource, const InteractionSourceActiveAction> (eSource);

  if ( cSourceAction.actionId == null_id )
    return;

  auto& callbackManager = registry.ctx().get <const CallbackManager> ();

  const auto eListener = cSource.listener;

  if ( entity_valid(eListener, registry) == false )
    return;

  auto& cListener = registry.get <const InteractionListener> (eListener);

  if ( cListener.actions.count(cSourceAction.actionId) == 0 )
    return;

  for ( const auto& callbackId : cListener.actions.at(cSourceAction.actionId) )
    callbackManager.execute(callbackId, registry, {eListener, eSource});
}

void
interactProbe(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using types::CallbackManager;
  using compos::InteractionProbe;
  using compos::InteractionSource;
  using compos::InteractionListener;

  const auto eProbe = std::any_cast <entt::entity> (args.at(0));
  const auto eListener = std::any_cast <entt::entity> (args.at(1));

  if ( eProbe == entt::null ||
       eListener == entt::null )
    return;

  auto& cListener = registry.get <const InteractionListener> (eListener);
  auto& cProbe = registry.get <const InteractionProbe> (eListener);

  const auto eSource = cProbe.listener.get(registry);

  if ( eSource == entt::null )
    return;

  auto& cSource = registry.get <InteractionSource> (eSource);

  for ( const auto& action : cSource.actions )
    if ( cListener.actions.count(action) > 0 )
    {
      cSource.listener = eListener;
      return;
    }
}


void
entityRemove(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityManager = registry.ctx().get <EntityManager> ();
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

  auto& entityManager = registry.ctx().get <EntityManager> ();

  const auto componentType =
    entityManager.componentType <SubscriberUpdate> ();

  entityManager.removeLater(entity, componentType);
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
  auto& entityManager = registry.ctx().get <const EntityManager> ();

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);

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

  auto& entityManager = registry.ctx().get <EntityManager> ();
  auto& entityList = registry.get <const EntityList> (entity);

  const auto componentType = entityManager.componentType <SubscriberUpdate> ();

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);
    entityManager.removeLater(entity, componentType);
  }
}

void
entitiesUpdateToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberUpdate;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);

    if ( entity != entt::null )
      entityUpdateToggle(registry, {entity});
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

  auto& entityManager = registry.ctx().get <EntityManager> ();

  const auto componentType =
    entityManager.componentType <SubscriberInput> ();

  entityManager.removeLater(entity, componentType);
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

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);

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

  auto& entityManager = registry.ctx().get <EntityManager> ();
  auto& entityList = registry.get <const EntityList> (entity);

  const auto componentType = entityManager.componentType <SubscriberInput> ();

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);

    if ( entity != entt::null )
      entityManager.removeLater(entity, componentType);
  }
}

void
entitiesInputToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::EntityManager;
  using compos::SubscriberInput;
  using compos::EntityList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& entityList = registry.get <const EntityList> (entity);

  for ( const auto& entityId : entityList.entities )
  {
    const auto entity = entityId.get(registry);

    if ( entity != entt::null )
      entityInputToggle(registry, {entity});
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

  auto& systemManager = registry.ctx().get <SystemManager> ();
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

  auto& systemManager = registry.ctx().get <SystemManager> ();
  auto& systemList = registry.get <const SystemList> (entity);

  for ( const auto& systemId : systemList.systems )
    systemManager.deactivate(systemId);
}

void
systemsToggle(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::SystemManager;
  using compos::SystemList;

  const auto entity = std::any_cast <entt::entity> (args.at(0));

  auto& systemManager = registry.ctx().get <SystemManager> ();
  auto& systemList = registry.get <const SystemList> (entity);

  for ( const auto& systemId : systemList.systems )
    if ( systemManager.isActive(systemId) == false )
      systemManager.activate(systemId);
    else
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

  registry.ctx().get <CallbackManager> ().executeLater(
  [] (  entt::registry& registry,
        const CallbackManager::CallbackArgs& )
  {
    try
    {
      const auto snapshotPath = registry.ctx().get <UserManager> ().snapshotsRoot();
      SnapshotManager::Load(registry, snapshotPath / "quick.json");
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR(e.what());
    }
  });
};


void
engineShutdown(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  using types::CasqadiumEngine;

  auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  glfwSetWindowShouldClose(engine.window(), true);
}

} // namespace cqde::callbacks
