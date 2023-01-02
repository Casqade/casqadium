#include <cqde/systems/physics.hpp>

#include <cqde/render_helpers.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/components/physics/CollisionBody.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_relational.hpp>


namespace cqde::systems
{

void
PhysicsSystem(
  entt::registry& registry )
{
  using compos::Transform;
  using compos::RigidBody;
  using compos::CollisionBody;
  using compos::SubscriberUpdate;
  using types::TickCurrent;
  using types::PhysicsManager;

  const auto& const_registry = registry;

  for ( auto&& [eBody, cBody]
          : registry.view <CollisionBody, SubscriberUpdate> ().each() )
    cBody.body->setIsActive(true);

  for ( auto&& [eBody, cBody]
          : registry.view <CollisionBody> (entt::exclude <SubscriberUpdate>).each() )
    cBody.body->setIsActive(false);

  for ( auto&& [eBody, cBody]
          : registry.view <RigidBody, SubscriberUpdate> ().each() )
    if ( cBody.body->isActive() == false )
    {
      cBody.body->setIsActive(true);
      cBody.body->setIsSleeping(false);
    }

  for ( auto&& [eBody, cBody]
          : registry.view <RigidBody> (entt::exclude <SubscriberUpdate>).each() )
    if ( cBody.body->isActive() == true )
    {
      cBody.body->setIsSleeping(true);
      cBody.body->setIsActive(false);
    }

//  todo: parallelize
  for ( auto&& [eBody, cTransform, cBody]
          : registry.view <const Transform, CollisionBody, const SubscriberUpdate> ().each() )
    cBody.body->setTransform(glmToRp3d(GetWorldMatrix(registry, eBody, cTransform)));

//  todo: parallelize
  for ( auto&& [eBody, cTransform, cBody]
          : registry.view <const Transform, RigidBody, const SubscriberUpdate> ().each() )
  {
    const auto transformCurrent = GetWorldMatrix(registry, eBody, cTransform);

    if ( glm::all(glm::equal(transformCurrent,
                             cBody.transformPrevious,
                             glm::epsilon <float> ())) == false )
      cBody.body->setTransform(glmToRp3d(transformCurrent));
  }

  auto& tick = const_registry.ctx().at <TickCurrent> ();
  const auto elapsed = tick.tickInterval;

  auto& physicsManager = const_registry.ctx().at <PhysicsManager> ();

  physicsManager.world()->update(static_cast <rp3d::decimal> (elapsed));

//  todo: sort transform by sceneNode depth ?

  for ( auto&& [eBody, cTransform, cBody]
          : registry.view <Transform, RigidBody, const SubscriberUpdate> ().each() )
  {
    auto transformCurrent = rp3dToGlm(cBody.body->getTransform());

    if ( glm::all(glm::equal(transformCurrent,
                             cBody.transformPrevious,
                             glm::epsilon <float> ())) == true )
      continue;

    cBody.transformPrevious = transformCurrent;

    transformCurrent = ToLocalSpace(transformCurrent, registry, eBody, cTransform);

    glm::vec3 nodeTranslation {};
    glm::quat nodeOrientation {};
    glm::vec3 nodeScale {};
    glm::vec3 skew {};
    glm::vec4 perspective {};

    glm::decompose( transformCurrent,
                    nodeScale,
                    nodeOrientation,
                    nodeTranslation,
                    skew, perspective );

    if ( glm::all(glm::isfinite(nodeTranslation)) == true &&
         glm::all(glm::isfinite(glm::eulerAngles(nodeOrientation))) == true &&
         glm::all(glm::isfinite(nodeScale)) == true )
    {
      cTransform.translation = nodeTranslation;
      cTransform.orientation = nodeOrientation;
      cTransform.scale = nodeScale;
    }
  }
}

void
PhysicsDebugRenderSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::SubscriberUpdate;
  using types::PhysicsManager;
  using DebugItem = rp3d::DebugRenderer::DebugItem;

  auto& physicsManager = registry.ctx().at <PhysicsManager> ();

  const auto world = physicsManager.world();
  auto& debugRenderer = world->getDebugRenderer();

  world->setIsDebugRenderingEnabled(true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_BROADPHASE_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLISION_SHAPE, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_POINT, true); // eats fps
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_NORMAL, true);
  debugRenderer.setNbSectorsSphere(6);
  debugRenderer.setNbStacksSphere(3);

  const auto& lines = debugRenderer.getLines();
  const auto& triangles = debugRenderer.getTriangles();

  if ( lines.size() == 0 &&
       triangles.size() == 0 )
    return;

  auto pge = olc::renderer->ptrPGE;

  for ( const auto&& [eCamera, cCamera, cCameraTransform]
          : registry.view <Camera, Transform, SubscriberUpdate> ().each() )
  {
    const glm::mat4 camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewportScaled();

    for ( const auto& line : lines )
    {
      std::vector <glm::vec3> buffer
      {
        rp3dToGlm(line.point1),
        rp3dToGlm(line.point2),
      };

      const auto vBuffer = vertexShader(
        buffer,
        camView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      olc::Pixel color = line.color1;
      color.a = 255;

      pge->DrawLineStripDecal(vBuffer.vertices, color);
    }

    for ( const auto& triangle : triangles )
    {
      std::vector <glm::vec3> buffer
      {
        rp3dToGlm(triangle.point1),
        rp3dToGlm(triangle.point2),
        rp3dToGlm(triangle.point3),
      };

      const auto vBuffer = vertexShader(
        buffer,
        camView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      olc::Pixel color = triangle.color1;
      color.a = 255;

      pge->DrawPolyLineDecal(vBuffer.vertices, color);
    }
  }
}

void
EditorPhysicsDebugRenderSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using types::PhysicsManager;
  using ui::ViewportManagerUi;
  using DebugItem = rp3d::DebugRenderer::DebugItem;

  auto& physicsManager = registry.ctx().at <PhysicsManager> ();
  auto& viewportManagerUi = registry.ctx().at <ViewportManagerUi> ();

  const auto world = physicsManager.world();
  auto& debugRenderer = world->getDebugRenderer();

  world->setIsDebugRenderingEnabled(true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_BROADPHASE_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLISION_SHAPE, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_POINT, true); // eats fps
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_NORMAL, true);
  debugRenderer.setNbSectorsSphere(6);
  debugRenderer.setNbStacksSphere(3);

  const auto& lines = debugRenderer.getLines();
  const auto& triangles = debugRenderer.getTriangles();

  if ( lines.size() == 0 &&
       triangles.size() == 0 )
    return;

  auto pge = olc::renderer->ptrPGE;

  for ( const auto& viewport : viewportManagerUi.viewports() )
  {
    const auto eCamera = viewport.camera.get_if_valid(registry);

    if ( eCamera == entt::null )
      continue;

    const auto [cCamera, cCameraTransform]
      = registry.try_get <Camera, Transform> (eCamera);

    if ( cCamera == nullptr ||
         cCameraTransform == nullptr )
      continue;

    const auto camView = cCamera->viewMatrix(registry, eCamera, *cCameraTransform);
    const auto camProjection = cCamera->projMatrix();
    const auto camViewport = viewport.viewport;

    for ( const auto& line : lines )
    {
      std::vector <glm::vec3> buffer
      {
        rp3dToGlm(line.point1),
        rp3dToGlm(line.point2),
      };

      const auto vBuffer = vertexShader(
        buffer,
        camView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      olc::Pixel color = line.color1;
      color.a = 255;

      pge->DrawLineStripDecal(vBuffer.vertices, color);
    }

    for ( const auto& triangle : triangles )
    {
      std::vector <glm::vec3> buffer
      {
        rp3dToGlm(triangle.point1),
        rp3dToGlm(triangle.point2),
        rp3dToGlm(triangle.point3),
      };

      const auto vBuffer = vertexShader(
        buffer,
        camView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      olc::Pixel color = triangle.color1;
      color.a = 255;

      pge->DrawPolyLineDecal(vBuffer.vertices, color);
    }
  }
};

} // namespace cqde::systems
