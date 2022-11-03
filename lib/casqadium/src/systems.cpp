#include <cqde/systems.hpp>

#include <cqde/callbacks.hpp>
#include <cqde/render_helpers.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/LightSource.hpp>
#include <cqde/components/LightTarget.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/SequenceController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/TextureTint.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <cqde/components/physics/CollisionBody.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_relational.hpp>

#include <imgui.h>


namespace cqde::systems
{

void
EditorSystem(
  entt::registry& registry )
{
  using namespace ui;
  using namespace compos;
  using types::InputManager;
  using types::EntityManager;
  using types::InputBindingRelative;

  static int32_t frameCountPrev {};
  const auto frameCount = ImGui::GetFrameCount();

  if ( frameCountPrev == frameCount )
    return;

  frameCountPrev = frameCount;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
  ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::PopStyleColor();

  callbacks::editorBindingsAssign(registry);
  callbacks::editorCameraCreate(registry);
  callbacks::editorControllerCreate(registry);

  registry.ctx().at <PackageManagerUi> ().ui_show(registry);
  registry.ctx().at <AssetManagerUi> ().ui_show(registry);
  registry.ctx().at <PrefabManagerUi> ().ui_show(registry);
  registry.ctx().at <EntityManagerUi> ().ui_show(registry);
  registry.ctx().at <InputManagerUi> ().ui_show(registry);
  registry.ctx().at <SystemManagerUi> ().ui_show(registry);
  registry.ctx().at <ViewportManagerUi> ().ui_show(registry);
}

void
EditorCullingSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::GeometryBuffer;
  using types::GeometryAssetManager;
  using ui::ViewportManagerUi;

  const auto& const_registry = registry;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();
  auto& viewportManagerUi = const_registry.ctx().at <ViewportManagerUi> ();

  for ( const auto& viewport : viewportManagerUi.viewports() )
  {
    const auto eCamera = viewport.camera.get_if_valid(registry);

    if ( eCamera == entt::null )
      continue;

    const auto [cCamera, cCameraTransform]
      = registry.try_get <Camera, const Transform> (eCamera);

    if ( cCamera == nullptr ||
         cCameraTransform == nullptr )
      continue;

    const auto camView = cCamera->viewMatrix(registry, eCamera, *cCameraTransform);
    const auto camProjection = cCamera->projMatrix();
    const auto camViewport = viewport.viewport;

    for ( const auto&& [eDrawable, cGeometryBuffer, cTransform]
            : registry.view <GeometryBuffer, Transform>().each() )
    {
      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView * GetWorldMatrix(registry, eDrawable, cTransform);

      const auto vBuffer = vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport,
        cCamera->zRange.first );

      if ( vBuffer.depth < 0.0f )
        continue;

      cCamera->zBuffer.emplace( vBuffer, eDrawable );
    }

    auto* pge = olc::renderer->ptrPGE;

    const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);
    const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);

    pge->DrawRectDecal({camViewport.x, camViewport.y},
                       {camViewport.z, camViewport.w},
                       colorViewport);

    const auto windowPos = viewport.window.pos;
    const auto windowSize = viewport.window.size;

    pge->DrawRectDecal({windowPos.x, windowPos.y},
                       {windowSize.x, windowSize.y},
                       colorWindow);
  }
};

void
EditorPhysicsDebugRenderSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using types::GeometryAssetManager;
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

      drawLines(vBuffer.vertices, color, LineRenderMode::Strip);
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

      olc::renderer->ptrPGE->DrawPolyLineDecal(vBuffer.vertices, color);
    }
  }
};

void
EditorEntityHighlightSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::CasqadiumEditorInternal;
  using types::EntityManager;
  using ui::EntityManagerUi;

  auto& entityManager   = registry.ctx().at <EntityManager> ();
  auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

  for ( const auto selectedEntity : entityManagerUi.selectedEntities() )
  {
    if ( selectedEntity == entt::null )
      return;

    for ( const auto&& [eCamera, cCamera]
            : registry.view <Camera, CasqadiumEditorInternal> ().each() )
      for ( const auto& [vBuf, entity] : cCamera.zBuffer )
        if ( entity == selectedEntity )
          olc::renderer->ptrPGE->DrawPolyLineDecal(vBuf.vertices, olc::YELLOW);
  }
};

void
MouseCenteringSystem(
  const entt::registry& registry )
{
  using namespace compos;

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    for ( const auto&& [entity] : registry.view <SubscriberInput, WantsMouseCentered> ().each() )
    {
      if ( olc::platform->ptrPGE->GetKeepMouseCentered() == false )
        olc::platform->ptrPGE->SetKeepMouseCentered(true);

      return;
    }

  else
    for ( const auto&& [entity] : registry.view <SubscriberInput, WantsMouseCentered, CasqadiumEditorInternal> ().each() )
    {
      if ( olc::platform->ptrPGE->GetKeepMouseCentered() == false )
        olc::platform->ptrPGE->SetKeepMouseCentered(true);

      return;
    }

  olc::platform->ptrPGE->SetKeepMouseCentered(false);
}

void
MouseHidingSystem(
  const entt::registry& registry )
{
  using namespace compos;

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    for ( const auto&& [entity] : registry.view <SubscriberInput, WantsMouseHidden> ().each() )
      return olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});

  else
    for ( const auto&& [entity] : registry.view <SubscriberInput, WantsMouseHidden, CasqadiumEditorInternal> ().each() )
      return olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});

  olc::platform->ptrPGE->ResetMouseCursor();
}

void
CullingSystem(
  entt::registry& registry )
{
  using namespace compos;
  using types::GeometryAssetManager;

  const auto& const_registry = registry;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraTransform]
          : registry.view <Camera, const Transform, const SubscriberUpdate> ().each() )
  {
    const glm::mat4 camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewportScaled();

    for ( const auto&& [eDrawable, cGeometryBuffer, cTransform]
            : registry.view <GeometryBuffer, Transform>().each() )
    {
      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView * GetWorldMatrix(registry, eDrawable, cTransform);

      const auto vBuffer = vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport,
        cCamera.zRange.first );

      if ( vBuffer.depth < 0.0f )
        continue;

      cCamera.zBuffer.emplace( vBuffer, eDrawable );
    }
  }
}

std::array <olc::vf2d, 4>
vec_to_array(
  const std::vector <olc::vf2d>& src )
{
  std::array <olc::vf2d, 4> dest;
  std::copy( src.begin(), src.begin() + 4, dest.begin() );

  return dest;
}

void
RenderSystem(
  entt::registry& registry )
{
  using namespace compos;
  using namespace types;

  const auto& const_registry = registry;

  registry.sort <Camera> (
  [] ( const Camera& lhs, const Camera& rhs )
  {
    return lhs.layer > rhs.layer;
  });

  for ( const auto&& [eCamera, cCamera]
          : const_registry.view <Camera> ().each() )
  {
    for ( const auto& [buffer, entity] : cCamera.zBuffer )
    {
      auto vertices = vec_to_array(buffer.vertices);

      if ( cCamera.renderMode == Camera::RenderMode::Solid )
      {
        olc::Decal* decal {};

        if ( cCamera.textureMode == Camera::TextureMode::Textured )
        {
          const TextureBuffer* textureBuffer = const_registry.try_get <TextureBuffer> (entity);
          if ( textureBuffer != nullptr )
          {
            if ( (textureBuffer->textures.size() > 0 &&
                  buffer.windingOrder == VertexBuffer::WindingOrder::CounterClockWise) ||
                 (textureBuffer->textures.size() > 1 &&
                  buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise) )
            {
              auto& textures  = registry.ctx().at <TextureAssetManager> ();
              const TextureId textureId = textureBuffer->textures.at((int) buffer.windingOrder);

              const auto texture = textures.get(textureId);
              decal = texture ? texture->Decal() : nullptr;

              if (  decal != nullptr &&
                    buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise )
                std::reverse(vertices.begin(), vertices.end());
            }
          }
        }

        if ( decal == nullptr )
          continue;

        auto tint = olc::WHITE;

        if (  cCamera.lightingMode == Camera::LightingMode::Diffuse &&
              registry.all_of <LightTarget> (entity) == true )
            tint = buffer.tint.front();

        else if ( auto cTextureTint = const_registry.try_get <TextureTint> (entity);
                  cTextureTint != nullptr )
          tint = cTextureTint->tint;

        olc::renderer->ptrPGE->DrawWarpedDecal( decal, vertices, tint );
        continue;
      }

      olc::renderer->ptrPGE->DrawPolyLineDecal(buffer.vertices, olc::GREY);
    }
  }
}

void
RenderBufferClearSystem(
  entt::registry& registry )
{
  using compos::Camera;

  for ( const auto&& [eCamera, cCamera]
          : registry.view <Camera> ().each() )
    cCamera.zBuffer.clear();
}

void
LightingSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::LightSource;
  using compos::LightTarget;
  using compos::TextureTint;
  using compos::Transform;
  using compos::SubscriberUpdate;

  const auto& const_registry = registry;

  for ( auto&& [eCamera, cCamera]
          : registry.view <Camera, const SubscriberUpdate> ().each() )
  {
    if ( cCamera.lightingMode != Camera::LightingMode::Diffuse )
      continue;

    for ( auto iter = cCamera.zBuffer.begin();
          iter != cCamera.zBuffer.end();
          ++iter )
    {
      auto& [buffer, eLightTgt] = *iter;

      if ( registry.all_of <LightTarget> (eLightTgt) == false )
        continue;

      const auto [cLightTgtTransform, cTextureTint]
        = const_registry.try_get <Transform, TextureTint> (eLightTgt);

      const auto lightTgtPos
        = GetWorldMatrix(registry, eLightTgt, *cLightTgtTransform)[3];

      olc::Pixel accumulatedLight {olc::BLACK};

      for ( const auto&& [eLightSrc, cLightSrcTransform, cLightSrc]
              : registry.view <const Transform, const LightSource, const SubscriberUpdate> ().each() )
      {
        const auto lightSrcPos
          = GetWorldMatrix(registry, eLightSrc, cLightSrcTransform)[3];

        const auto distance = glm::length(lightTgtPos - lightSrcPos);

        if ( distance >= cLightSrc.radius )
          continue;

        const float attenuation = 1.0f
          / ( cLightSrc.attenuationConstant
            + cLightSrc.attenuationLinear * distance
            + cLightSrc.attenuationQuadratic * std::pow(distance, 2) );

        glm::vec3 normal {};
        glm::vec3 lightDir {};

        if ( cLightSrc.type != LightSource::Type::Ambient )
          normal = glm::rotate( ToWorldSpace( cLightTgtTransform->orientation,
                                              registry,
                                              eLightTgt, *cLightTgtTransform),
                                {0.0f, 0.0f, 1.0f});

        switch (cLightSrc.type)
        {
          case LightSource::Type::Ambient:
          {
            accumulatedLight += cLightSrc.color * attenuation;
            break;
          }
          case LightSource::Type::Positional:
          {
            lightDir = glm::normalize(lightSrcPos - lightTgtPos);

            const float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

            accumulatedLight += cLightSrc.color * attenuation * diffuse;
            break;
          }
          case LightSource::Type::Directional:
          {
            lightDir = glm::rotate( ToWorldSpace( cLightSrcTransform.orientation,
                                                  registry,
                                                  eLightTgt, cLightSrcTransform),
                                    {0.0f, 0.0f, 1.0f});

            const float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

            accumulatedLight += cLightSrc.color * attenuation * diffuse;
            break;
          }
        }
      }

      const glm::vec4 resultLight
      {
        accumulatedLight.r / 255.0f,
        accumulatedLight.g / 255.0f,
        accumulatedLight.b / 255.0f,
        1.0f
      };

      glm::vec4 resultTint {1.0f};

      if ( cTextureTint != nullptr )
        resultTint =
        {
          cTextureTint->tint.r / 255.0f,
          cTextureTint->tint.g / 255.0f,
          cTextureTint->tint.b / 255.0f,
          cTextureTint->tint.a / 255.0f,
        };

      resultTint *= resultLight;

      auto vb = buffer;
      auto& tint = vb.tint.front();

      tint = olc::PixelF( resultTint.r, resultTint.g,
                          resultTint.b, resultTint.a );

      cCamera.zBuffer.erase(iter);
      iter = cCamera.zBuffer.emplace(vb, eLightTgt);
    }
  }
}

void
SequenceSystem(
  entt::registry& registry )
{
  using compos::SubscriberUpdate;
  using compos::SequenceController;

  for ( auto&& [entity, cSequenceController]
          : registry.view <SequenceController, SubscriberUpdate> ().each() )
  {
    if ( cSequenceController.steps.empty() == true )
      continue;

    auto step = cSequenceController.steps.front().get();

    if ( step->execute(registry, entity) == true )
    {
      cSequenceController.steps.pop_front();

      if ( cSequenceController.steps.empty() == true )
        continue;

      step = cSequenceController.steps.front().get();

      step->init(registry, entity);
    }
  }
}

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
      drawLines(vBuffer.vertices, color, LineRenderMode::Strip);
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
      olc::renderer->ptrPGE->DrawPolyLineDecal(vBuffer.vertices, color);
    }
  }
}

} // namespace cqde::systems
