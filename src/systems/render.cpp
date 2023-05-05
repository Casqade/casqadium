#include <cqde/systems/render.hpp>

#include <cqde/render_helpers.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/SystemManager.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/MeshAssetManager.hpp>

#include <cqde/types/graphics/GlBuffer.hpp>
#include <cqde/types/graphics/GlVertexArray.hpp>
#include <cqde/types/graphics/ShaderManager.hpp>
#include <cqde/types/graphics/FrameReadback.hpp>
#include <cqde/types/graphics/PrimaryRenderTarget.hpp>

#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/DrawableMesh.hpp>
#include <cqde/components/InteractionListener.hpp>
#include <cqde/components/InteractionListenerColor.hpp>
#include <cqde/components/InteractionSource.hpp>
#include <cqde/components/LightSource.hpp>
#include <cqde/components/LightTarget.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/TextureTint.hpp>

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

  glViewport( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glScissor( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

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

  struct OutlineBuffer
  {
    GlVertexArray vao {};
    GlBuffer vertices {};

    void create()
    {
      using AttributeType = glm::vec2;

      vertices.create(
        sizeof(AttributeType) * 8,
        GlBuffer::MutableStorageFlags(),
        nullptr );

      vao.create();
      vao.enableAttribute(0);
      vao.setAttributeFormat(
        0, AttributeType::length(),
        0, GL_FLOAT );

      vao.attachBuffer(
        vertices, 0, 0, 0,
        sizeof(AttributeType) );
    }

    void destroy()
    {
      vao.destroy();
      vertices.destroy();
    }
  };

  OutlineBuffer outline {};


  EditorRenderData()
  {
    outline.create();
  }

  ~EditorRenderData()
  {
    outline.destroy();
  }
};

static void
GeometryPass(
  entt::registry& registry,
  const glm::mat4& viewProjection,
  const types::ShaderType shaderType )
{
  using compos::Camera;
  using compos::Transform;
  using compos::DrawableMesh;
  using compos::TextureTint;
  using compos::SubscriberUpdate;
  using compos::InteractionListener;
  using types::ShaderManager;
  using types::SystemManager;
  using types::MeshAssetManager;
  using types::TextureAssetManager;
  using SystemPhase = types::System::Phase;

  struct RenderCommand
  {
    glm::mat4 mvp {};

    GLuint textureId {};

    size_t firstElement {};
    size_t elementCount {};
    size_t firstVertex {};

    GLuint objectId {};

    uint32_t tint {};
  };

  auto& meshes = registry.ctx().get <MeshAssetManager> ();
  auto& textures  = registry.ctx().get <TextureAssetManager> ();
  auto& shaderManager = registry.ctx().get <ShaderManager> ();

  auto& shader = shaderManager.get(shaderType);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");
  const auto uObjectId = shader.uniformLocation("uObjectId");
  const auto uTextureTint = shader.uniformLocation("uTextureTint");

  static std::vector <RenderCommand> renderCommands {};
  renderCommands.clear();

  for ( const auto&& [eDrawable, cTransform, cMesh]
          : registry.view <Transform, DrawableMesh> (entt::exclude <TextureTint>).each() )
  {
    const auto mesh = meshes.get(cMesh.meshId);

    if ( mesh == nullptr )
      continue;

    const auto texture = textures.get(cMesh.textureId);

    if ( texture == nullptr ||
         texture->Decal() == nullptr )
      continue;

    renderCommands.emplace_back( RenderCommand
    {
      viewProjection * GetWorldMatrix(registry, eDrawable, cTransform),
      texture->Decal()->id,
      mesh->firstElementIndex,
      mesh->elementCount,
      mesh->firstVertexIndex,
      static_cast <GLuint> (eDrawable),
      olc::WHITE.n,
    } );
  }

  std::sort( renderCommands.begin(), renderCommands.end(),
  [] ( const RenderCommand& lhs, const RenderCommand& rhs )
  {
//    front-to-back
    return lhs.mvp[3].z < rhs.mvp[3].z;
  });

  const auto lastOpaqueCommandIndex = renderCommands.size();

  for ( const auto&& [eDrawable, cTransform, cMesh, cTextureTint]
          : registry.view <Transform, DrawableMesh, TextureTint> ().each() )
  {
    const auto mesh = meshes.get(cMesh.meshId);

    if ( mesh == nullptr )
      continue;

    const auto texture = textures.get(cMesh.textureId);

    if ( texture == nullptr ||
         texture->Decal() == nullptr )
      continue;

    renderCommands.emplace_back( RenderCommand
    {
      viewProjection * GetWorldMatrix(registry, eDrawable, cTransform),
      texture->Decal()->id,
      mesh->firstElementIndex,
      mesh->elementCount,
      mesh->firstVertexIndex,
      static_cast <GLuint> (eDrawable),
      cTextureTint.tint.n,
    } );
  };

  std::sort( renderCommands.begin() + lastOpaqueCommandIndex, renderCommands.end(),
  [] ( const RenderCommand& lhs, const RenderCommand& rhs )
  {
//    back-to-front
    return lhs.mvp[3].z > rhs.mvp[3].z;
  });

  GLuint textureIdCurrent {};

  glBindVertexArray(meshes.vaoId());

  for ( auto& command : renderCommands )
  {
    if ( command.textureId != textureIdCurrent )
    {
      textureIdCurrent = command.textureId;
      glBindTextureUnit(0, textureIdCurrent);
    }

    glUniformMatrix4fv(
      uTransform, 1, GL_FALSE,
      glm::value_ptr(command.mvp) );

    glUniform1ui(uObjectId, command.objectId);
    glUniform1ui(uTextureTint, command.tint);

    const auto elementOffset = sizeof(uint32_t) * command.firstElement;

    glDrawElementsBaseVertex(
      GL_TRIANGLES,
      command.elementCount,
      GL_UNSIGNED_INT,
      reinterpret_cast <const void*> (elementOffset),
      command.firstVertex );
  }


//  Rerender interactive objects using fully opaque texture.
//  Otherwise transparent object parts won't be rendered at all
//  and thus won't be detected by interaction system

  auto& systemManager = registry.ctx().get <SystemManager> ();

  if ( systemManager.systemsActive(SystemPhase::Editor).empty() == true )
  {
    renderCommands.clear();

    for ( const auto&& [eDrawable, cTransform, cMesh, cListener]
      : registry.view <Transform, DrawableMesh, InteractionListener, SubscriberUpdate> ().each() )
    {
      const auto mesh = meshes.get(cMesh.meshId);

      if ( mesh == nullptr )
        continue;

      renderCommands.emplace_back( RenderCommand
      {
        viewProjection * GetWorldMatrix(registry, eDrawable, cTransform),
        0,
        mesh->firstElementIndex,
        mesh->elementCount,
        mesh->firstVertexIndex,
        static_cast <GLuint> (eDrawable),
      } );
    }

    std::sort( renderCommands.begin(), renderCommands.end(),
    [] ( const RenderCommand& lhs, const RenderCommand& rhs )
    {
//      front-to-back
      return lhs.mvp[3].z < rhs.mvp[3].z;
    });

    glDisable(GL_CULL_FACE);
    glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBindTextureUnit(0, 0);
    glUniform1ui(uTextureTint, olc::WHITE.n);

    for ( auto& command : renderCommands )
    {
      glUniformMatrix4fv(
      uTransform, 1, GL_FALSE,
      glm::value_ptr(command.mvp) );

      glUniform1ui(uObjectId, command.objectId);

      const auto elementOffset = sizeof(uint32_t) * command.firstElement;

      glDrawElementsBaseVertex(
      GL_TRIANGLES,
      command.elementCount,
      GL_UNSIGNED_INT,
      reinterpret_cast <const void*> (elementOffset),
      command.firstVertex );
    }

    glEnable(GL_CULL_FACE);
    glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }

  glBindVertexArray(0);
  glBindTextureUnit(0, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void
DrawViewportOutline(
  entt::registry& registry,
  const types::RenderTarget& target,
  const glm::vec4& viewport )
{
  using types::ShaderType;
  using types::ShaderManager;

  const glm::vec2 framebufferSize = target.size;

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
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

  const auto& outline = renderData.outline;

  const std::vector <glm::vec2> viewportOutline
  {
    {1.0f, viewport.w},
    {1.0f, 1.0f},
    {viewport.z - 0.0f, 1.0f},
    {viewport.z - 0.0f, viewport.w},
  };

  const std::vector <glm::vec2> windowOutline
  {
    {1.0f, framebufferSize.y},
    {1.0f, 1.0f},
    {framebufferSize.x - 0.0f, 1.0f},
    {framebufferSize.x - 0.0f, framebufferSize.y},
  };

  const auto viewportOutlineSize =
    sizeof(decltype(viewportOutline)::value_type) * viewportOutline.size();

  const auto windowOutlineSize =
    sizeof(decltype(windowOutline)::value_type) * windowOutline.size();

  outline.vertices.write( 0,
    viewportOutlineSize,
    viewportOutline.data() );

  outline.vertices.write(
    viewportOutlineSize,
    windowOutlineSize,
    windowOutline.data() );

  outline.vao.bind();

  auto projection = glm::ortho(
    0.0f, viewport.z,
    0.0f, viewport.w,
    0.0f, 1.0f );

  glUniform1ui(uColor, colorViewport);
  glUniformMatrix4fv(
    uTransform,
    1, GL_FALSE,
    glm::value_ptr(projection) );

  glDrawArrays(GL_LINE_LOOP, 0, 4);

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

  glDrawArrays(GL_LINE_LOOP, 4, 4);

  glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glBindVertexArray(0);
}

static void
ClearRenderTarget(
  const types::RenderTarget& target )
{
  const glm::vec2 framebufferSize = target.size;

  glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);

  glViewport( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glScissor( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glClearNamedFramebufferfv( target.fbo,
    GL_COLOR, 0,
    glm::value_ptr(glm::vec4{0.0, 0.0, 0.0, 1.0f}) );

  glClearNamedFramebufferfi( target.fbo,
    GL_DEPTH_STENCIL, 0,
    1.0f, 0 );

  const uint32_t invalidId {entt::null};

  glClearNamedFramebufferuiv( target.fbo,
    GL_COLOR, 1, &invalidId );
}

static void
RenderToTarget(
  entt::registry& registry,
  const types::RenderTarget& target,
  const entt::entity eCamera )
{
  using compos::Camera;
  using compos::Transform;
  using types::ShaderType;
  using types::ShaderManager;
  using types::FrameReadbackQueue;

  if ( eCamera == entt::null )
    return;

  const auto& const_registry = registry;

  const auto&& [cCamera, cCameraTransform]
    = const_registry.try_get <Camera, Transform> (eCamera);

  if ( cCamera == nullptr ||
       cCameraTransform == nullptr )
    return;

  const glm::vec2 framebufferSize = target.size;
  const auto camViewport = cCamera->viewportScaled(framebufferSize);

  glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
  glEnable(GL_SCISSOR_TEST);

  glViewport( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glScissor( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glEnablei(GL_BLEND, 0);
  glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(
    camViewport.x, camViewport.y,
    camViewport.z, camViewport.w );

  glScissor(
    camViewport.x, camViewport.y,
    camViewport.z, camViewport.w );

  const auto camView = cCamera->viewMatrix(const_registry, eCamera, *cCameraTransform);
  const auto viewProjection = cCamera->projMatrix(framebufferSize) * camView;

  ShaderType shaderType = ShaderType::Geometry;

  if ( cCamera->renderMode == Camera::RenderMode::Solid )
  {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  }
  else
  {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    shaderType = ShaderType::Wireframe;
  }

  GeometryPass(registry, viewProjection, shaderType);

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();
  readbackQueue.write(target);
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
  using ui::ViewportManagerUi;

  const auto& const_registry = registry;

  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();

  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();

  for ( auto& viewport : viewportManagerUi.viewports() )
    if ( viewport.visible == true )
    {
      const auto eCamera = viewport.camera.get(registry);

      ClearRenderTarget(viewport.framebuffer);

      RenderToTarget(
        registry,
        viewport.framebuffer,
        eCamera );

      readbackQueue.process(registry);

      if ( eCamera == entt::null )
        continue;

      const auto&& [cCamera, cCameraTransform]
        = const_registry.try_get <Camera, Transform> (eCamera);

      if ( cCamera == nullptr ||
           cCameraTransform == nullptr )
        continue;

      DrawViewportOutline(
        registry,
        viewport.framebuffer,
        cCamera->viewportScaled(viewport.framebuffer.size) );
    }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_BLEND);
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
  using types::ShaderType;
  using types::ShaderManager;
  using types::CasqadiumEngine;
  using types::FrameReadbackQueue;
  using types::PrimaryRenderTarget;
  using compos::Camera;
  using compos::Transform;
  using compos::SubscriberUpdate;

  registry.sort <Camera> (
  [] ( const Camera& lhs, const Camera& rhs )
  {
    return lhs.layer > rhs.layer;
  });

  auto& engine = *registry.ctx().get <CasqadiumEngine*> ();
  auto& mainTarget = registry.ctx().get <PrimaryRenderTarget> ();
  auto& readbackQueue = registry.ctx().get <FrameReadbackQueue> ();

  const auto framebufferSize = engine.framebufferSize();

  if ( mainTarget.target.update(framebufferSize) == true )
    readbackQueue.clear();

  auto& shaderManager = registry.ctx().get <ShaderManager> ();
  auto& shader = shaderManager.get(ShaderType::FullscreenQuad);

  ClearRenderTarget(mainTarget.target);

  for ( const auto&& [eCamera, cCamera, cTransform]
          : registry.view <Camera, Transform, SubscriberUpdate> ().each() )
    RenderToTarget(registry, mainTarget.target, eCamera);

  readbackQueue.write(mainTarget.target);
  readbackQueue.process(registry);

  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_BLEND);

  glViewport( 0, 0,
    framebufferSize.x,
    framebufferSize.y );

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTextureUnit(0, mainTarget.target.textureAlbedo.id());
  mainTarget.vao.bind();

  shader.use();

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

  glBindTextureUnit(0, 0);
  shader.unuse();
}

void
EditorEntityHighlightSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::DrawableMesh;
  using compos::CasqadiumEditorInternal;
  using types::ShaderType;
  using types::ShaderManager;
  using types::MeshAssetManager;
  using ui::EntityManagerUi;
  using ui::ViewportManagerUi;

  const auto& const_registry = registry;

  auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();
  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();

  auto& meshes = registry.ctx().get <MeshAssetManager> ();
  auto& shaderManager = registry.ctx().get <ShaderManager> ();

  auto& shader = shaderManager.get(ShaderType::UiElements);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");
  const auto uColor = shader.uniformLocation("uColor");

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glUniform1ui(uColor, olc::YELLOW.n);

  glBindVertexArray(meshes.vaoId());

  glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

  for ( const auto selectedEntity : entityManagerUi.selectedEntities() )
  {
    if ( selectedEntity == entt::null )
      continue;

    auto&& [cTransform, cMesh] =
      const_registry.try_get <Transform, DrawableMesh> (selectedEntity);

    if ( cTransform == nullptr ||
         cMesh == nullptr )
      continue;

    const auto mesh = meshes.get(cMesh->meshId);

    if ( mesh == nullptr )
      continue;

    for ( auto& viewport : viewportManagerUi.viewports() )
    {
      if ( viewport.visible == false )
        continue;

      const auto eCamera = viewport.camera.get(registry);

      auto&& [cCamera, cCameraTransform] = registry.try_get <Camera, Transform> (eCamera);

      if ( cCamera == nullptr ||
           cCameraTransform == nullptr )
        continue;

      const auto framebufferSize = viewport.framebuffer.size;
      const auto camViewport = cCamera->viewportScaled(framebufferSize);

      glBindFramebuffer(GL_FRAMEBUFFER, viewport.framebuffer.fbo);

      glViewport(
        camViewport.x, camViewport.y,
        camViewport.z, camViewport.w );

      glScissor(
        camViewport.x, camViewport.y,
        camViewport.z, camViewport.w );

      const auto camView = cCamera->viewMatrix(const_registry, eCamera, *cCameraTransform);
      const auto viewProjection = cCamera->projMatrix(framebufferSize) * camView;

      const auto mvp = viewProjection * GetWorldMatrix(
        registry,
        selectedEntity,
        *cTransform );

      glUniformMatrix4fv(
        uTransform, 1, GL_FALSE,
        glm::value_ptr(mvp) );

      const auto elementOffset = sizeof(uint32_t) * mesh->firstElementIndex;

      glDrawElementsBaseVertex(
        GL_TRIANGLES,
        mesh->elementCount,
        GL_UNSIGNED_INT,
        reinterpret_cast <const void*> (elementOffset),
        mesh->firstVertexIndex );
    }
  }

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  shader.unuse();

  glBindVertexArray(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
InteractionHighlightSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::InteractionListenerColor;
  using compos::InteractionSource;
  using compos::SubscriberUpdate;
  using compos::DrawableMesh;
  using compos::Transform;
  using types::ShaderType;
  using types::ShaderManager;
  using types::PrimaryRenderTarget;
  using types::MeshAssetManager;

  const auto& const_registry = registry;

  auto& meshes = registry.ctx().get <MeshAssetManager> ();
  auto& shaderManager = registry.ctx().get <ShaderManager> ();
  auto& mainTarget = registry.ctx().get <PrimaryRenderTarget> ();

  auto& shader = shaderManager.get(ShaderType::UiElements);
  shader.use();

  const auto uTransform = shader.uniformLocation("uTransform");
  const auto uColor = shader.uniformLocation("uColor");

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindVertexArray(meshes.vaoId());

  for ( const auto&& [eCamera, cCamera, cCameraTransform, cInteractionSource]
          : registry.view <Camera, Transform, InteractionSource, SubscriberUpdate> ().each() )
  {
    const auto eListener = cInteractionSource.listener;

    if ( entity_valid(eListener, registry) == false )
      continue;

    auto&& [cTransform, cMesh, cListenerColor] =
      const_registry.try_get <Transform, DrawableMesh, InteractionListenerColor> (eListener);

    if ( cTransform == nullptr ||
         cMesh == nullptr ||
         cListenerColor == nullptr )
      continue;

    const auto mesh = meshes.get(cMesh->meshId);

    if ( mesh == nullptr )
      continue;

    const auto framebufferSize = mainTarget.target.size;
    const auto camViewport = cCamera.viewportScaled(framebufferSize);

    glViewport(
      camViewport.x, camViewport.y,
      camViewport.z, camViewport.w );

    glScissor(
      camViewport.x, camViewport.y,
      camViewport.z, camViewport.w );

    const auto camView = cCamera.viewMatrix(registry, eCamera, cCameraTransform);
    const auto viewProjection = cCamera.projMatrix(framebufferSize) * camView;

    const auto mvp = viewProjection * GetWorldMatrix(
      registry,
      eListener,
      *cTransform );

    glUniform1ui(uColor, cListenerColor->color.n);
    glUniformMatrix4fv(
      uTransform, 1, GL_FALSE,
      glm::value_ptr(mvp) );

    const auto elementOffset = sizeof(uint32_t) * mesh->firstElementIndex;

    glDrawElementsBaseVertex(
      GL_LINE_LOOP,
      mesh->elementCount,
      GL_UNSIGNED_INT,
      reinterpret_cast <const void*> (elementOffset),
      mesh->firstVertexIndex );
  }

  shader.unuse();
  glBindVertexArray(0);
}

} // namespace cqde::systems
