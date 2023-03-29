#include <cqde/systems/render.hpp>

#include <cqde/render_helpers.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/graphics/GlBuffer.hpp>
#include <cqde/types/graphics/GlVertexArray.hpp>
#include <cqde/types/graphics/ShaderManager.hpp>
#include <cqde/types/graphics/FrameReadback.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
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

#include <ctpl/ctpl_stl.h>

#include <glm/gtc/type_ptr.hpp>

#include <glad/gl.h>


namespace cqde::systems
{

void
RenderBufferClearSystem(
  entt::registry& registry )
{
  using types::CasqadiumEngine;

  auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  const auto framebufferSize = engine.framebufferSize();

  glViewport(0, 0, framebufferSize.x, framebufferSize.y);
  glScissor(0, 0, framebufferSize.x, framebufferSize.y);

  glClearNamedFramebufferfv( 0,
    GL_COLOR, 0,
    glm::value_ptr(glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}) );

  glClearNamedFramebufferfi( 0,
    GL_DEPTH_STENCIL, 0,
    1.0f, 0 );
}

struct EditorRenderData
{
  using GlBuffer = types::GlBuffer;
  using GlVertexArray = types::GlVertexArray;

  struct LineBuffer
  {
    GlVertexArray vao {};
    GlBuffer vertices {};

    void create()
    {
      using AttributeType = glm::vec2;

      vertices.create(
        sizeof(AttributeType) * 4,
        GlBuffer::MutableStorageFlags(),
        nullptr );

      vao.create();
      vao.enableAttribute(0);
      vao.setAttributeFormat(
        0, AttributeType::length(),
        0, GL_FLOAT );

      vao.attachBuffer(
        vertices, 0, 0,
        sizeof(AttributeType) );
    }

    void destroy()
    {
      vao.destroy();
      vertices.destroy();
    }
  };

  LineBuffer viewportOutline {};
  LineBuffer windowOutline {};


  EditorRenderData()
  {
    viewportOutline.create();
    windowOutline.create();
  }

  ~EditorRenderData()
  {
    viewportOutline.destroy();
    windowOutline.destroy();
  }
};

void
geometryPass(
  entt::registry& registry,
  const GLenum renderMode,
  const glm::mat4& viewProjection )
{
  using compos::Camera;
  using compos::Transform;
  using compos::GeometryBuffer;
  using compos::TextureBuffer;
  using types::ShaderType;
  using types::ShaderManager;
  using types::GeometryAssetManager;
  using types::TextureAssetManager;

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();
  auto& textures  = registry.ctx().get <TextureAssetManager> ();
  auto& shaderManager = registry.ctx().get <ShaderManager> ();

  auto& shader = shaderManager.get(ShaderType::Geometry);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");
  const auto uObjectId = shader.uniformLocation("uObjectId");
  const auto aTexCoords = shader.attribLocation("aTexCoords");

  GLuint geometryIdCurrent {};
  GLuint textureIdCurrent {};
  GLuint uvIdCurrent {};

  for ( const auto&& [eDrawable, cTransform, cGeometryBuffer, cTextureBuffer]
          : registry.view <Transform, GeometryBuffer, TextureBuffer> ().each() )
  {
    const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

    if ( gBuffer == nullptr )
      return;

    const auto mvp = viewProjection * GetWorldMatrix(registry, eDrawable, cTransform);

    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1ui(uObjectId, static_cast <uint32_t> (eDrawable));

    if ( gBuffer->vao.id() != geometryIdCurrent )
    {
      geometryIdCurrent = gBuffer->vao.id();
      glBindVertexArray(geometryIdCurrent);
      glEnableVertexAttribArray(1);
    }

    if ( cTextureBuffer.uv != uvIdCurrent )
    {
      uvIdCurrent = cTextureBuffer.uv;
      glBindBuffer(GL_ARRAY_BUFFER, uvIdCurrent);
      glVertexAttribPointer(
        aTexCoords, 2, GL_FLOAT,
        GL_FALSE, 0, 0 );
    }

    const auto texture = textures.get(cTextureBuffer.texture);

    if ( texture != nullptr &&
         texture->Decal() != nullptr &&
         texture->Decal()->id != textureIdCurrent )
    {
      textureIdCurrent = texture->Decal()->id;
      glBindTextureUnit(0, textureIdCurrent);
    }

    glDrawElements( renderMode,
      gBuffer->indices.size(),
      GL_UNSIGNED_INT, nullptr );
  };

  glBindVertexArray(0);
  glBindTextureUnit(0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
EditorRenderSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using types::ShaderType;
  using types::ShaderManager;
  using types::CallbackManager;
  using types::FrameReadbackQueue;
  using types::FrameReadbackResult;
  using ui::ViewportManagerUi;

  const auto& const_registry = registry;

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
  auto& callbackManager = registry.ctx().get <CallbackManager> ();
  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();

  for ( auto& viewport : viewportManagerUi.viewports() )
  {
    if ( viewport.visible == false )
      continue;

    glBindFramebuffer(GL_FRAMEBUFFER, viewport.framebuffer.fbo);

    glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glEnable(GL_SCISSOR_TEST);

    const glm::vec2 framebufferSize = viewport.framebuffer.size;

    glViewport( 0, 0,
      framebufferSize.x,
      framebufferSize.y );

    glScissor( 0, 0,
      framebufferSize.x,
      framebufferSize.y );

    glClearNamedFramebufferfv( viewport.framebuffer.fbo,
      GL_COLOR, 0,
      glm::value_ptr(glm::vec4{0.0, 0.0, 0.0, 1.0f}) );

    glClearNamedFramebufferfi( viewport.framebuffer.fbo,
      GL_DEPTH_STENCIL, 0,
      1.0f, 0 );

    const uint32_t invalidId {entt::null};

    glClearNamedFramebufferuiv( viewport.framebuffer.fbo,
      GL_COLOR, 1, &invalidId );

    const auto eCamera = viewport.camera.get(registry);

    if ( eCamera == entt::null )
      continue;

    const auto&& [cCamera, cCameraTransform]
      = const_registry.try_get <Camera, Transform> (eCamera);

    if ( cCamera == nullptr ||
         cCameraTransform == nullptr )
      continue;

    const auto camViewport = cCamera->viewportScaled(framebufferSize);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    const auto camView = cCamera->viewMatrix(const_registry, eCamera, *cCameraTransform);
    const auto viewProjection = cCamera->projMatrix(framebufferSize) * camView;

    GLenum renderMode = GL_LINE_LOOP;

    if ( cCamera->renderMode == Camera::RenderMode::Solid )
    {
      glEnable(GL_DEPTH_TEST);
      renderMode = GL_TRIANGLE_FAN;
    }
    else
      glDisable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);

    geometryPass(registry, renderMode, viewProjection);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    if ( registry.ctx().contains <EditorRenderData> () == false )
      registry.ctx().emplace <EditorRenderData> ();

    auto& renderData = registry.ctx().get <EditorRenderData> ();

    auto& shaderManager = registry.ctx().get <ShaderManager> ();

    auto& shader = shaderManager.get(ShaderType::UiElements);
    shader.use();

    const auto uTransform = shader.uniformLocation("uTransform");
    const auto uColor = shader.uniformLocation("uColor");

    const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);
    const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);

    const auto& viewportOutline = renderData.viewportOutline;
    const auto& windowOutline = renderData.windowOutline;

    const std::vector <glm::vec2> viewportOutlineVertices
    {
      {1.0f, camViewport.w},
      {1.0f, 1.0f},
      {camViewport.z - 0.0f, 1.0f},
      {camViewport.z - 0.0f, camViewport.w},
    };

    const std::vector <glm::vec2> windowOutlineVertices
    {
      {1.0f, framebufferSize.y},
      {1.0f, 1.0f},
      {framebufferSize.x - 0.0f, 1.0f},
      {framebufferSize.x - 0.0f, framebufferSize.y},
    };

    viewportOutline.vertices.write( 0,
      sizeof(decltype(viewportOutlineVertices)::value_type) * viewportOutlineVertices.size(),
      viewportOutlineVertices.data() );

    windowOutline.vertices.write( 0,
      sizeof(decltype(windowOutlineVertices)::value_type) * windowOutlineVertices.size(),
      windowOutlineVertices.data() );

    viewportOutline.vao.bind();

    auto projection = glm::ortho(
      0.0f, camViewport.z,
      0.0f, camViewport.w,
      -1.0f, 1.0f );

    glUniform1ui(uColor, colorViewport);
    glUniformMatrix4fv(
      uTransform,
      1, GL_FALSE,
      glm::value_ptr(projection) );

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    windowOutline.vao.bind();

    glViewport(
      0, 0,
      framebufferSize.x,
      framebufferSize.y );

    glScissor(
      0, 0,
      framebufferSize.x,
      framebufferSize.y );

    projection = glm::ortho(
      0.0f, framebufferSize.x,
      0.0f, framebufferSize.y,
      0.0f, 1.0f );

    glUniform1ui(uColor, colorWindow);
    glUniformMatrix4fv(
      uTransform, 1, GL_FALSE,
      glm::value_ptr(projection));

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);

    auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();

    for ( auto& request : readbackQueue.requests )
      if ( request.fence == nullptr &&
           request.framebufferId == viewport.framebuffer.fbo )
      {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, request.buffers.first.id());

        glNamedFramebufferReadBuffer(
          viewport.framebuffer.fbo,
          GL_COLOR_ATTACHMENT1 );

        glReadPixels(
          request.pos.x, request.pos.y,
          request.size.x, request.size.y,
          GL_RED_INTEGER,
          GL_UNSIGNED_INT,
          nullptr );

        const auto bufferSize =
          sizeof(decltype(request.data)::value_type) * request.size.x * request.size.y;

        glCopyNamedBufferSubData(
          request.buffers.first.id(),
          request.buffers.second.id(),
          0, 0,
          bufferSize );

        request.fence = glFenceSync(
          GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
      }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_BLEND);

  while ( readbackQueue.requests.empty() == false )
  {
    auto& request = readbackQueue.requests.front();

    if ( request.fence == nullptr )
      break;

    GLint status {};
    GLsizei length {};

    glGetSynciv( request.fence,
      GL_SYNC_STATUS, sizeof(status),
      &length, &status );

    if ( status != GL_SIGNALED )
      break;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, request.buffers.first.id());

    const auto readDataSize =
      sizeof(decltype(request.data)::value_type) * request.size.x * request.size.y;

    const auto readData = request.buffers.second.map(
      0, readDataSize,
      GL_MAP_READ_BIT );

    std::memcpy(
      request.data.data(),
      readData, readDataSize );

    request.buffers.second.unmap();

    if ( request.callback != nullptr )
    {
      const FrameReadbackResult result
      {
        request.pos,
        request.size,
        std::move(request.data),
      };

      callbackManager.executeLater(
        request.callback, {result} );
    }

    readbackQueue.pop();
  }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
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
//    if ( cCamera.lightingMode != Camera::LightingMode::Diffuse )
//      continue;

//    for ( auto iter = cCamera.zBuffer.begin();
//          iter != cCamera.zBuffer.end();
//          ++iter )
//    {
//      const auto [buffer, eLightTgt] = *iter;

//      if ( registry.all_of <LightTarget> (eLightTgt) == false )
//        continue;

//      const auto [cLightTgtTransform, cTextureTint]
//        = const_registry.try_get <Transform, TextureTint> (eLightTgt);

//      const auto lightTgtPos
//        = GetWorldMatrix(registry, eLightTgt, *cLightTgtTransform)[3];

//      std::mutex accumulatedLightMutex {};
//      olc::Pixel accumulatedLight {olc::BLACK};

//      const auto lightSourcesView
//          = const_registry.view <Transform, LightSource, SubscriberUpdate> ();

//      const auto& handle = lightSourcesView.handle();

//      std::for_each(std::execution::par_unseq, handle.cbegin(), handle.cend(),
//      [&, cLightTgtTransform = cLightTgtTransform, eLightTgt = eLightTgt] ( const auto eLightSrc )
//      {
//        if ( lightSourcesView.contains(eLightSrc) == false )
//          return;

//        const auto&& [cLightSrcTransform, cLightSrc]
//          = lightSourcesView.get <const Transform, const LightSource> (eLightSrc);

//        const auto lightSrcPos
//          = GetWorldMatrix(registry, eLightSrc, cLightSrcTransform)[3];

//        const auto distance = glm::length(lightTgtPos - lightSrcPos);

//        if ( distance > cLightSrc.radius )
//          return;

//        const float attenuation = 1.0f
//          / ( cLightSrc.attenuationConstant
//            + cLightSrc.attenuationLinear * distance
//            + cLightSrc.attenuationQuadratic * std::pow(distance, 2) );

//        glm::vec3 normal {};
//        glm::vec3 lightDir {};

//        if ( cLightSrc.type != LightSource::Type::Ambient )
//          normal = glm::rotate( ToWorldSpace( cLightTgtTransform->orientation,
//                                              registry,
//                                              eLightTgt, *cLightTgtTransform),
//                                {0.0f, 0.0f, 1.0f});

//        switch (cLightSrc.type)
//        {
//          case LightSource::Type::Ambient:
//          {
//            std::lock_guard lock {accumulatedLightMutex};
//            accumulatedLight += cLightSrc.color * attenuation;
//            break;
//          }
//          case LightSource::Type::Positional:
//          {
//            lightDir = glm::normalize(lightSrcPos - lightTgtPos);

//            const float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

//            std::lock_guard lock {accumulatedLightMutex};
//            accumulatedLight += cLightSrc.color * attenuation * diffuse;

//            break;
//          }
//          case LightSource::Type::Directional:
//          {
//            lightDir = glm::rotate( ToWorldSpace( cLightSrcTransform.orientation,
//                                                  registry,
//                                                  eLightTgt, cLightSrcTransform),
//                                    {0.0f, 0.0f, 1.0f});

//            const float diffuse = glm::max(glm::dot(normal, lightDir), 0.0f);

//            std::lock_guard lock {accumulatedLightMutex};
//            accumulatedLight += cLightSrc.color * attenuation * diffuse;

//            break;
//          }
//        }
//      });

//      const glm::vec4 resultLight
//      {
//        accumulatedLight.r / 255.0f,
//        accumulatedLight.g / 255.0f,
//        accumulatedLight.b / 255.0f,
//        1.0f
//      };

//      glm::vec4 resultTint {1.0f};

//      if ( cTextureTint != nullptr )
//        resultTint =
//        {
//          cTextureTint->tint.r / 255.0f,
//          cTextureTint->tint.g / 255.0f,
//          cTextureTint->tint.b / 255.0f,
//          cTextureTint->tint.a / 255.0f,
//        };

//      resultTint *= resultLight;
//    }
  }
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

//    for ( const auto&& [eCamera, cCamera]
//            : registry.view <Camera, CasqadiumEditorInternal> ().each() )
//      for ( const auto& [vBuf, entity] : cCamera.zBuffer )
//        if ( entity == selectedEntity )
//          olc::renderer->ptrPGE->DrawPolyLineDecal(vBuf.vertices, olc::YELLOW);
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

//    for ( const auto& [vBuf, entity] : cCamera.zBuffer )
//      if ( entity == eListener )
//      {
//        olc::renderer->ptrPGE->DrawPolyLineDecal(vBuf.vertices, cListenerColor->color);
//        break;
//      }
  }
}

} // namespace cqde::systems
