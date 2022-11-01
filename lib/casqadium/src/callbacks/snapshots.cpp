#include <cqde/callbacks/snapshots.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/SnapshotManager.hpp>
#include <cqde/types/UserManager.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

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
