#include <cqde/common.hpp>

#include <cqde/ecs_helpers.hpp>

#include <cqde/components/InputController.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityTagManager.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde
{

void
engineInit( entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace cqde::types;

//  entt::meta_ctx::bind(registry.ctx().emplace <entt::meta_ctx> ());

  registry.ctx().emplace <TickCurrent> ();
  registry.ctx().emplace <FrameCurrent> ();

  registry.ctx().emplace <PackageManager> ();

  auto& entityManager = registry.ctx().emplace <EntityManager> ();
  registry.ctx().emplace <EntityTagManager> ();
  registry.ctx().emplace <CallbackManager> ();

  registry.ctx().emplace <InputManager> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <GeometryAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);

  entityManager.registerComponent <SceneNode> ("SceneNode");
  entityManager.registerComponent <InputController> ("InputController");
  entityManager.registerComponent <Transform> ("Transform");
  entityManager.registerComponent <GeometryBuffer> ("GeometryBuffer");
  entityManager.registerComponent <TextureBuffer> ("TextureBuffer");
  entityManager.registerComponent <Camera> ("Camera");
}

} // namespace cqde
