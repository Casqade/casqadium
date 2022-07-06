#include <cqde/common.hpp>

#include <cqde/ecs_helpers.hpp>

#include <cqde/components/InputController.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <cqde/types/EntityTagManager.hpp>
#include <cqde/types/PackageManager.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/input/InputCallbackStorage.hpp>
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

  component_register <Tag> ("Tag");
  component_register <SceneNode> ("SceneNode");
  component_register <InputController> ("InputController");
  component_register <Transform> ("Transform");
  component_register <GeometryBuffer> ("GeometryBuffer");
  component_register <TextureBuffer> ("TextureBuffer");
  component_register <Camera> ("Camera");

  registry.ctx().emplace <PackageManager> ();

  registry.ctx().emplace <EntityManager> ();
  registry.ctx().emplace <EntityTagManager> ();
  registry.ctx().emplace <InputCallbackStorage> ();

  registry.ctx().emplace <InputManager> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);
}

} // namespace cqde
