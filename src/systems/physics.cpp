#include <cqde/systems/physics.hpp>

#include <cqde/assert.hpp>
#include <cqde/render_helpers.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/graphics/GlProgram.hpp>
#include <cqde/types/graphics/GlVertexArray.hpp>
#include <cqde/types/graphics/ShaderManager.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/components/physics/CollisionBody.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_relational.hpp>

#include <glad/gl.h>


namespace cqde::types
{

struct PhysicsDebugDrawData
{
  using GlBuffer = types::GlBuffer;
  using GlVertexArray = types::GlVertexArray;

  struct DataBuffer
  {
    using PositionType = glm::vec3;
    using ColorType = uint32_t;

    GlVertexArray vao {};
    GlBuffer vertices {};

    size_t elementCount {};


    void create()
    {
      const auto colorOffset = sizeof(PositionType);

      vao.create();
      vao.enableAttribute(0);
      vao.setAttributeFormat(
        0, PositionType::length(),
        0, GL_FLOAT );

      vao.enableAttribute(1);
      vao.setAttributeFormat(
        1, 3,
        colorOffset, GL_UNSIGNED_BYTE,
        GlVertexArray::AttributeType::Float,
        true );

      resize(1);
    }

    void destroy()
    {
      vao.destroy();
      vertices.destroy();
    }

    void resize( const size_t newElementCount )
    {
      CQDE_ASSERT_DEBUG(vao.isValid() == true, return);

      if ( newElementCount <= elementCount )
        return;

      const auto stride = sizeof(PositionType) + sizeof(ColorType);
      const auto colorOffset = sizeof(PositionType);

      elementCount = newElementCount;

      const auto sizeBytes = elementCount * stride;

      if ( vertices.isValid() == true )
      {
        vao.detachBuffer(vertices);
        vertices.destroy();
      }

      vertices.create(
        sizeBytes,
        GlBuffer::MutableStorageFlags(),
        nullptr );

      vao.attachBuffer(
        vertices, 0, 0,
        0, stride );

      vao.attachBuffer(
        vertices, 1, 0,
        0, stride );
    }
  };

  DataBuffer buffer {};


  PhysicsDebugDrawData()
  {
    buffer.create();
  }

  ~PhysicsDebugDrawData()
  {
    buffer.destroy();
  }
};

}

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

  for ( auto&& [eBody, cTransform, cBody]
          : registry.view <const Transform, CollisionBody, const SubscriberUpdate> ().each() )
    cBody.body->setTransform(glmToRp3d(GetWorldMatrix(registry, eBody, cTransform)));

  for ( auto&& [eBody, cTransform, cBody]
          : registry.view <const Transform, RigidBody, const SubscriberUpdate> ().each() )
  {
    const auto transformCurrent = GetWorldMatrix(registry, eBody, cTransform);

    if ( glm::all(glm::equal(transformCurrent,
                             cBody.transformPrevious,
                             glm::epsilon <float> ())) == false )
      cBody.body->setTransform(glmToRp3d(transformCurrent));
  }

  auto& tick = const_registry.ctx().get <TickCurrent> ();
  const auto elapsed = tick.tickInterval;

  auto& physicsManager = const_registry.ctx().get <PhysicsManager> ();

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
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::SubscriberUpdate;
  using types::CasqadiumEngine;
  using types::GlProgram;
  using types::ShaderType;
  using types::ShaderManager;
  using types::PhysicsManager;
  using types::PhysicsDebugDrawData;
  using DebugItem = rp3d::DebugRenderer::DebugItem;

  const auto& physicsManager = registry.ctx().get <PhysicsManager> ();

  const auto world = physicsManager.world();
  auto& debugRenderer = world->getDebugRenderer();

  world->setIsDebugRenderingEnabled(true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_BROADPHASE_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLISION_SHAPE, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_POINT, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_NORMAL, true);
//  debugRenderer.setNbSectorsSphere(6);
//  debugRenderer.setNbStacksSphere(3);

  const auto& lines = debugRenderer.getLines();
  const auto& triangles = debugRenderer.getTriangles();

  if ( lines.size() == 0 &&
       triangles.size() == 0 )
    return;

  const auto engine = registry.ctx().get <CasqadiumEngine*> ();

  auto& shaderManager = registry.ctx().get <ShaderManager> ();
  auto& shader = shaderManager.get(ShaderType::DebugDraw);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");

  if ( registry.ctx().contains <PhysicsDebugDrawData> () == false )
    registry.ctx().emplace <PhysicsDebugDrawData> ();

  auto& debugDrawData = registry.ctx().get <PhysicsDebugDrawData> ();

  const auto debugVertexCount = lines.size() * 2 + triangles.size() * 3;

  if ( debugVertexCount > debugDrawData.buffer.elementCount )
    debugDrawData.buffer.resize(debugVertexCount);

  const auto linesSize = sizeof(*lines.begin()) * lines.size();
  const auto trianglesSize = sizeof(*triangles.begin()) * triangles.size();

  debugDrawData.buffer.vertices.write(
    0, linesSize,
    &*lines.begin() );

  debugDrawData.buffer.vertices.write(
    linesSize, trianglesSize,
    &*triangles.begin() );

  debugDrawData.buffer.vao.bind();

  glEnable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  const auto framebufferSize = engine->framebufferSize();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glViewport(
    0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glScissor(
    0, 0,
    framebufferSize.x,
    framebufferSize.y );

  for ( const auto&& [eCamera, cCamera, cCameraTransform]
          : registry.view <Camera, Transform, SubscriberUpdate> ().each() )
  {
    const auto camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
    const auto camProjection = cCamera.projMatrix(framebufferSize) * camView;

    glUniformMatrix4fv( uTransform,
      1, GL_FALSE,
      glm::value_ptr(camProjection) );

    glDrawArrays( GL_LINES,
      0, lines.size() * 2 );

    glDrawArrays( GL_TRIANGLES,
      lines.size() * 2,
      triangles.size() * 3 );
  }

  debugDrawData.buffer.vao.unbind();

  glDisable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void
EditorPhysicsDebugRenderSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using types::ShaderType;
  using types::ShaderManager;
  using types::PhysicsManager;
  using types::PhysicsDebugDrawData;
  using ui::ViewportManagerUi;
  using DebugItem = rp3d::DebugRenderer::DebugItem;

  auto& physicsManager = registry.ctx().get <PhysicsManager> ();
  const auto& viewportManagerUi = registry.ctx().get <ViewportManagerUi> ();

  const auto world = physicsManager.world();
  auto& debugRenderer = world->getDebugRenderer();

  world->setIsDebugRenderingEnabled(true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLIDER_BROADPHASE_AABB, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::COLLISION_SHAPE, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_POINT, true);
  debugRenderer.setIsDebugItemDisplayed(DebugItem::CONTACT_NORMAL, true);
//  debugRenderer.setNbSectorsSphere(6);
//  debugRenderer.setNbStacksSphere(3);

  const auto& lines = debugRenderer.getLines();
  const auto& triangles = debugRenderer.getTriangles();

  if ( lines.size() == 0 &&
       triangles.size() == 0 )
    return;

  auto& shaderManager = registry.ctx().get <ShaderManager> ();
  auto& shader = shaderManager.get(ShaderType::DebugDraw);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");

  if ( registry.ctx().contains <PhysicsDebugDrawData> () == false )
    registry.ctx().emplace <PhysicsDebugDrawData> ();

  auto& debugDrawData = registry.ctx().get <PhysicsDebugDrawData> ();

  const auto debugVertexCount = lines.size() * 2 + triangles.size() * 3;

  if ( debugVertexCount > debugDrawData.buffer.elementCount )
    debugDrawData.buffer.resize(debugVertexCount);

  const auto linesSize = sizeof(*lines.begin()) * lines.size();
  const auto trianglesSize = sizeof(*triangles.begin()) * triangles.size();

  debugDrawData.buffer.vertices.write(
    0, linesSize,
    &*lines.begin() );

  debugDrawData.buffer.vertices.write(
    linesSize, trianglesSize,
    &*triangles.begin() );

  debugDrawData.buffer.vao.bind();

  glEnable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  for ( const auto& viewport : viewportManagerUi.viewports() )
  {
    const auto eCamera = viewport.camera.get(registry);

    if ( eCamera == entt::null )
      continue;

    const auto [cCamera, cCameraTransform]
      = registry.try_get <Camera, Transform> (eCamera);

    if ( cCamera == nullptr ||
         cCameraTransform == nullptr )
      continue;

    const auto framebufferSize = viewport.framebuffer.size;
    const auto camViewport = cCamera->viewportScaled(framebufferSize);

    const auto camView = cCamera->viewMatrix(registry, eCamera, *cCameraTransform);
    const auto camProjection = cCamera->projMatrix(framebufferSize) * camView;

    glBindFramebuffer(GL_FRAMEBUFFER, viewport.framebuffer.fbo);

    glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glViewport(
      camViewport.x,
      camViewport.y,
      camViewport.z,
      camViewport.w );

    glScissor(
      camViewport.x,
      camViewport.y,
      camViewport.z,
      camViewport.w );

    glUniformMatrix4fv( uTransform,
      1, GL_FALSE,
      glm::value_ptr(camProjection) );

    glDrawArrays( GL_LINES,
      0, lines.size() * 2 );

    glDrawArrays( GL_TRIANGLES,
      lines.size() * 2,
      triangles.size() * 3 );
  }

  debugDrawData.buffer.vao.unbind();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_SCISSOR_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
};

} // namespace cqde::systems
