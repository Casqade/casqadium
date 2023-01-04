#include <cqde/systems/render.hpp>

#include <cqde/render_helpers.hpp>

#include <cqde/types/EntityManager.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/InteractionListenerColor.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/LightSource.hpp>
#include <cqde/components/LightTarget.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/TextureTint.hpp>
#include <cqde/components/TextureBuffer.hpp>

#include <entt/entity/registry.hpp>

#include <execution>


namespace cqde::systems
{

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
CullingSystem(
  entt::registry& registry )
{
  using namespace compos;
  using types::GeometryAssetManager;

  const auto& const_registry = registry;

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraTransform]
          : registry.view <Camera, const Transform, const SubscriberUpdate> ().each() )
  {
    const glm::mat4 camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewportScaled();

    std::recursive_mutex zBufferMutex {};

    auto drawablesView = const_registry.view <GeometryBuffer, Transform> ();

    const auto& handle = drawablesView.handle();

    std::for_each(std::execution::par_unseq, handle.begin(), handle.end(),
    [&, cCamera = &cCamera] ( const auto eDrawable )
    {
      if ( drawablesView.contains(eDrawable) == false )
        return;

      const auto&& [cGeometryBuffer, cTransform]
        = drawablesView.get <const GeometryBuffer, const Transform> (eDrawable);

      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        return;

      const auto modelView = camView * GetWorldMatrix(registry, eDrawable, cTransform);

      const auto vBuffer = vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport,
        cCamera->zRange.first );

      if ( vBuffer.depth < 0.0f )
        return;

      std::lock_guard lock {zBufferMutex};
      cCamera->zBuffer.emplace( vBuffer, eDrawable );
    });
  }
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

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();
  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();

  for ( const auto& viewport : viewportManagerUi.viewports() )
  {
    const auto eCamera = viewport.camera.get(registry);

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

    std::recursive_mutex zBufferMutex {};

    auto drawablesView = const_registry.view <GeometryBuffer, Transform> ();

    const auto& handle = drawablesView.handle();

    std::for_each(std::execution::par_unseq, handle.begin(), handle.end(),
    [&, cCamera = cCamera] ( const auto eDrawable )
    {
      if ( drawablesView.contains(eDrawable) == false )
        return;

      const auto&& [cGeometryBuffer, cTransform]
        = drawablesView.get <const GeometryBuffer, const Transform> (eDrawable);

      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        return;

      const auto modelView = camView * GetWorldMatrix(registry, eDrawable, cTransform);

      const auto vBuffer = vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport,
        cCamera->zRange.first );

      if ( vBuffer.depth < 0.0f )
        return;

      std::lock_guard lock {zBufferMutex};
      cCamera->zBuffer.emplace( vBuffer, eDrawable );
    });

    auto pge = olc::renderer->ptrPGE;

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
      const auto [buffer, eLightTgt] = *iter;

      if ( registry.all_of <LightTarget> (eLightTgt) == false )
        continue;

      const auto [cLightTgtTransform, cTextureTint]
        = const_registry.try_get <Transform, TextureTint> (eLightTgt);

      const auto lightTgtPos
        = GetWorldMatrix(registry, eLightTgt, *cLightTgtTransform)[3];

      std::mutex accumulatedLightMutex {};
      olc::Pixel accumulatedLight {olc::BLACK};

      const auto lightSourcesView
          = const_registry.view <Transform, LightSource, SubscriberUpdate> ();

      const auto& handle = lightSourcesView.handle();

      std::for_each(std::execution::par_unseq, handle.begin(), handle.end(),
      [&, cLightTgtTransform = cLightTgtTransform, eLightTgt = eLightTgt] ( const auto eLightSrc )
      {
        if ( lightSourcesView.contains(eLightSrc) == false )
          return;

        const auto&& [cLightSrcTransform, cLightSrc]
          = lightSourcesView.get <const Transform, const LightSource> (eLightSrc);

        const auto lightSrcPos
          = GetWorldMatrix(registry, eLightSrc, cLightSrcTransform)[3];

        const auto distance = glm::length(lightTgtPos - lightSrcPos);

        if ( distance > cLightSrc.radius )
          return;

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
            std::lock_guard lock {accumulatedLightMutex};
            accumulatedLight += cLightSrc.color * attenuation;
            break;
          }
          case LightSource::Type::Positional:
          {
            lightDir = glm::normalize(lightSrcPos - lightTgtPos);

            const float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

            std::lock_guard lock {accumulatedLightMutex};
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

            std::lock_guard lock {accumulatedLightMutex};
            accumulatedLight += cLightSrc.color * attenuation * diffuse;

            break;
          }
        }
      });

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
RenderSystem(
  entt::registry& registry )
{
  using namespace compos;
  using namespace types;

  const auto& const_registry = registry;

  auto pge = olc::renderer->ptrPGE;

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
              auto& textures  = registry.ctx().get <TextureAssetManager> ();
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

        pge->DrawWarpedDecal( decal, vertices, tint );
        continue;
      }

      pge->DrawPolyLineDecal(buffer.vertices, olc::GREY);
    }
  }
}

void
EditorEntityHighlightSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::CasqadiumEditorInternal;
  using types::EntityManager;
  using ui::EntityManagerUi;

  auto& entityManager   = registry.ctx().get <EntityManager> ();
  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();

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
}

void
InteractionHighlightSystem(
  const entt::registry& registry )
{
  using compos::Camera;
  using compos::InteractionListenerColor;
  using compos::InteractionSource;
  using compos::SubscriberUpdate;

  for ( const auto&& [eCamera, cCamera, cInteractionSource]
          : registry.view <Camera, InteractionSource, SubscriberUpdate> ().each() )
  {
    const auto eListener = cInteractionSource.listener;

    if ( entity_valid(eListener, registry) == false )
      continue;

    const auto cListenerColor = registry.try_get <InteractionListenerColor> (eListener);

    if ( cListenerColor == nullptr )
      continue;

    for ( const auto& [vBuf, entity] : cCamera.zBuffer )
      if ( entity == eListener )
      {
        olc::renderer->ptrPGE->DrawPolyLineDecal(vBuf.vertices, cListenerColor->color);
        break;
      }
  }
}

} // namespace cqde::systems
