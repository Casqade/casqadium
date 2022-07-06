#include <cqde/common.hpp>

#include <cqde/ecs_helpers.hpp>

#include <cqde/components/InputController.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/EntityTagManager.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputCallbackStorage.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <entt/entity/registry.hpp>


#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde
{

void
engineInit( entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  InputController::Register();
  SceneNode::Register();
  Tag::Register();
  Transform::Register();

  registry.ctx().emplace <PackageManager> ();

  registry.ctx().emplace <EntityTagManager> ();
  registry.ctx().emplace <InputCallbackStorage> ();

  registry.ctx().emplace <InputManager> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);
}

} // namespace cqde
