#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/logger.hpp>

#include <cqde/components/SnapshotExcluded.hpp>

#include <entt/entity/registry.hpp>

#include <soloud.h>

#include <json/writer.h>

#include <fstream>


namespace cqde::types
{

const static Json::Value snapshotReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// snapshot root must be a JSON object"s,
                       Json::CommentPlacement::commentBefore);

  return reference;
}();

void
SnapshotManager::Validate(
  const Json::Value& snapshot )
{
  jsonValidateObject(snapshot, snapshotReference);

  EntityManager::Validate(snapshot["registry"]);
  SystemManager::Validate(snapshot["systems"]);
}

void
SnapshotManager::Validate(
  const path& snapshotPath )
{
  using fmt::format;

  Json::Value snapshot {};

  LOG_TRACE("Parsing snapshot '{}'",
            snapshotPath.string());

  try
  {
    snapshot = fileParse(snapshotPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse snapshot ({})",
              e.what()));
  }

  LOG_TRACE("Validating snapshot '{}'",
            snapshotPath.string());

  try
  {
    Validate(snapshot);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate snapshot '{}': {}",
              snapshotPath.string(), e.what()));
  }
}

void
SnapshotManager::refreshSnapshotList(
  entt::registry& registry )
{
  mSnapshots.clear();

  const auto& userManager = registry.ctx().get <UserManager> ();

  const auto rootPath = userManager.snapshotsRoot();

  for ( const auto& snapshotEntry : std::filesystem::directory_iterator{rootPath} )
  {
    if ( snapshotEntry.is_regular_file() == false )
      continue;

    Snapshot snapshot
    {
      snapshotEntry.path(),
      snapshotEntry.last_write_time(),
    };

    mSnapshots.push_back(snapshot);
  }
}

std::vector <Snapshot>
SnapshotManager::snapshots() const
{
  return mSnapshots;
}

Json::Value
SnapshotManager::Create(
  const entt::registry& registry )
{
  using namespace compos;

  const auto& entityManager = registry.ctx().get <EntityManager> ();
  const auto& systemManager = registry.ctx().get <SystemManager> ();

  const std::unordered_set <ComponentType> excludedComponents
  {
    entityManager.componentType <Tag> (),
    entityManager.componentType <SnapshotExcluded> (),
  };

  const auto registrySnapshot
    = entityManager.serialize( "", registry, excludedComponents );

  const auto systemsSnapshot = systemManager.serialize();

  Json::Value snapshot {Json::objectValue};

  snapshot["registry"] = registrySnapshot;
  snapshot["systems"] = systemsSnapshot;

  return snapshot;
}

void
SnapshotManager::Write(
  const entt::registry& registry,
  const std::string& filename )
{
  const auto& userManager = registry.ctx().get <UserManager> ();

  const auto snapshotPath = userManager.snapshotsRoot() / filename;

  const auto snapshot = Create(registry);

  LOG_INFO("Writing snapshot to '{}'",
           snapshotPath.string());

  const auto streamFlags = std::ios::out |
                           std::ios::trunc |
                           std::ios::binary;

  try
  {
    auto fileStream = fileOpen(snapshotPath, streamFlags);
    fileStream << Json::writeString(jsonWriter(), snapshot);
  }
  catch ( const std::exception& e )
  {
    using fmt::format;

    throw std::runtime_error(
      format("Failed to write snapshot to '{}': {}",
             snapshotPath.string(), e.what()));
  }
}

void
SnapshotManager::Load(
  entt::registry& registry,
  const path& snapshotPath )
{
  using fmt::format;
  using ui::EntityManagerUi;
  using ui::SystemManagerUi;
  using Phase = types::System::Phase;

  Json::Value snapshot {};

  LOG_TRACE("Parsing snapshot '{}'",
            snapshotPath.string());

  try
  {
    snapshot = fileParse(snapshotPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse snapshot ({})",
              e.what()));
  }

  LOG_TRACE("Validating snapshot '{}'",
            snapshotPath.string());

  try
  {
    Validate(snapshot);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate snapshot '{}': {}",
              snapshotPath.string(), e.what()));
  }

  registry.ctx().get <SoLoud::Soloud> ().stopAll();

  auto& entityManager = registry.ctx().get <EntityManager> ();

  if ( registry.ctx().contains <EntityManagerUi> () == true )
  {
    auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

    entityManagerUi.entitiesDeselect();
    entityManagerUi.componentDeselect();
  }

  entityManager.clear();
  registry.clear();

  LOG_TRACE("Loading snapshot '{}'",
            snapshotPath.string());

  try
  {
    const auto& packageManager = registry.ctx().get <PackageManager> ();

    for ( const auto& packageId : packageManager.packages() )
    {
      auto package = packageManager.package(packageId);

      if ( package == nullptr )
        throw std::runtime_error(
          format("Failed to load package '{}' entity registry: "
                 "No such package in PackageManager",
                 packageId.str()));

      using ContentType = types::Package::ContentType;

      entityManager.load( package->contentPath(ContentType::Entities),
                          packageId, registry);
    }

    entityManager.entryPointExecute(registry);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to load snapshot '{}': {}",
             snapshotPath.string(), e.what()));
  }

//  todo: replace with registry.view <void> when it becomes available
//  for ( const auto&& [entity] : registry.view <void> (entt::exclude <compos::SnapshotExcluded> ).each() )
//    entityManager.removeLater(entity);
  registry.each(
  [&registry, &entityManager] ( const entt::entity entity )
  {
    if ( registry.all_of <compos::SnapshotExcluded> (entity) == false )
      entityManager.removeLater(entity);
  });

  auto& registrySnapshot = snapshot["registry"];
  auto& systemsSnapshot = snapshot["systems"];

  try
  {
    entityManager.deserialize(registrySnapshot, "", registry);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to deserialize snapshot '{}': {}",
              snapshotPath.string(), e.what()));
  }

  auto& systemManager = registry.ctx().get <SystemManager> ();

  try
  {
    systemManager.deserialize(systemsSnapshot);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to deserialize snapshot '{}': {}",
              snapshotPath.string(), e.what()));
  }

  if ( registry.ctx().contains <SystemManagerUi> () == true )
    registry.ctx().get <SystemManagerUi> ().init(registry);

  if ( systemManager.systemsActive(Phase::Editor).empty() == true )
    return;

  systemManager.deactivate();

  for ( const auto& systemId : systemManager.systems(Phase::Editor) )
    systemManager.activate(systemId);
}

} // namespace cqde::types
