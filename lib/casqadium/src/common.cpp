#include <cqde/common.hpp>

#include <cqde/ecs_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/SnapshotExcluded.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/SequenceController.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>


#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/FrameCurrent.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/CallbackManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <entt/entity/registry.hpp>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde
{

void
engineInit( entt::registry& registry )
{
  using namespace compos;
  using namespace types;
  using namespace ui;

//  entt::meta_ctx::bind(registry.ctx().emplace <entt::meta_ctx> ());

  registry.ctx().emplace <TickCurrent> ();
  registry.ctx().emplace <FrameCurrent> ();

  auto& callbackManager = registry.ctx().emplace <CallbackManager> ();
  auto& entityManager = registry.ctx().emplace <EntityManager> ();
  auto& inputManager = registry.ctx().emplace <InputManager> ();
  auto& packageManager = registry.ctx().emplace <PackageManager> ();
  auto& systemManager = registry.ctx().emplace <SystemManager> ();

  registry.ctx().emplace <AssetManagerUi> ();
  registry.ctx().emplace <CallbackManagerUi> (&callbackManager);
  registry.ctx().emplace <EntityManagerUi> (&entityManager);
  registry.ctx().emplace <InputManagerUi> (&inputManager);
  registry.ctx().emplace <PackageManagerUi> (&packageManager);
  registry.ctx().emplace <SystemManagerUi> (&systemManager);
  registry.ctx().emplace <ViewportManagerUi> ();

  auto& tp = registry.ctx().emplace <ctpl::thread_pool> (std::thread::hardware_concurrency() | 1);

  registry.ctx().emplace <FontAssetManager> (tp);
  registry.ctx().emplace <GeometryAssetManager> (tp);
  registry.ctx().emplace <TextureAssetManager> (tp);
  registry.ctx().emplace <TextStringAssetManager> (tp);

  entityManager.registerComponent <Camera> ("Camera");
  entityManager.registerComponent <EntityMetaInfo> ("EntityMetaInfo");
  entityManager.registerComponent <GeometryBuffer> ("GeometryBuffer");
  entityManager.registerComponent <InputController> ("InputController");
  entityManager.registerComponent <SceneNode> ("SceneNode");
  entityManager.registerComponent <SequenceController> ("SequenceController");
  entityManager.registerComponent <Tag> ("Tag");
  entityManager.registerComponent <TextureBuffer> ("TextureBuffer");
  entityManager.registerComponent <Transform> ("Transform");

//  Tags
  entityManager.registerEmptyComponent <SnapshotExcluded> ("SnapshotExcluded");
  entityManager.registerEmptyComponent <SubscriberInput> ("SubscriberInput");
  entityManager.registerEmptyComponent <SubscriberUpdate> ("SubscriberUpdate");
}

std::string
toLowerCase(
  const std::string& str )
{
  std::string result = str;

  for ( auto& c : result )
    c = std::tolower(c);

  return result;
}

std::vector <std::string>
stringSplit(
  const std::string& string,
  const char delimeter )
{
  std::vector <std::string> result {};

  std::stringstream stream {string};
  std::string token {};

  while ( getline (stream, token, delimeter) )
    result.push_back(token);

  return result;
}

std::string stringJoin(
  const std::vector <std::string>& strings,
  const std::string& delimeter )
{
  std::string result {};

  for ( auto iter = strings.begin();
        iter != strings.end();
        ++iter )
    if ( iter == strings.end() - 1 )
      result += *iter;
    else
      result += *iter + delimeter;

  return result;
}

} // namespace cqde
