#include <cqde/systems/render.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/types/EntityManager.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/graphics/GlProgram.hpp>

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

#include <ctpl/ctpl_stl.h>

#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

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

    });
  }
}

struct EditorRenderData
{
  olc::Renderable nullTexture;

  GLuint uvBufferFront;
  GLuint uvBufferBack;

  GLuint lineBuffer;


  EditorRenderData()
  {
    nullTexture.Create(1, 1);
    nullTexture.Sprite()->SetPixel({}, olc::WHITE);
    nullTexture.Decal()->Update();


    const std::vector <glm::vec2> uvFront
    {
      {0.0f, 0.0f},
      {0.0f, 1.0f},
      {1.0f, 1.0f},
      {1.0f, 0.0f},
    };

    glGenBuffers(1, &uvBufferFront);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferFront);

    glBufferData(GL_ARRAY_BUFFER, uvFront.size() * sizeof(decltype(uvFront)::value_type),
                 uvFront.data(), GL_STATIC_DRAW);


    const std::vector <glm::vec2> uvBack
    (
      uvFront.rbegin(),
      uvFront.rend()
    );

    glGenBuffers(1, &uvBufferBack);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferBack);

    glBufferData(GL_ARRAY_BUFFER, uvBack.size() * sizeof(decltype(uvBack)::value_type),
                 uvBack.data(), GL_STATIC_DRAW);


    glGenBuffers(1, &lineBuffer);
  }

  ~EditorRenderData()
  {
    glDeleteBuffers(1, &uvBufferFront);
    glDeleteBuffers(1, &uvBufferBack);
    glDeleteBuffers(1, &lineBuffer);
  }
};

//  immediate
void
EditorCullingSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::GeometryBuffer;
  using compos::TextureBuffer;
  using types::GlProgram;
  using types::GeometryAssetManager;
  using types::TextureAssetManager;
  using ui::ViewportManagerUi;

  const auto& const_registry = registry;

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();
  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();
  auto& textures  = registry.ctx().get <TextureAssetManager> ();

  static std::vector <glm::mat4> worldMatrices {};
  worldMatrices.resize(registry.storage <GeometryBuffer> ().size());

  if ( viewportManagerUi.viewports().empty() == false )
  {
    auto& tp = registry.ctx().get <ctpl::thread_pool> ();

    auto drawablesView = const_registry.view <GeometryBuffer, Transform> ();
    const auto& handle = drawablesView.handle();

    const auto sortGeometry = tp.push(
    [&registry] ( const int32_t threadId )
    {
      registry.sort <GeometryBuffer> (
      [] ( const GeometryBuffer& lhs, const GeometryBuffer& rhs )
      {
        return lhs.buffer < rhs.buffer;
      });
    });

//    const auto sortTextures = tp.push(
//    [&registry] ( const int32_t threadId )
//    {
//      registry.sort <TextureBuffer> (
//      [] ( const TextureBuffer& lhs, const TextureBuffer& rhs )
//      {
//        return lhs. < rhs.buffer;
//      });
//    });

    sortGeometry.wait();

    std::for_each(std::execution::par_unseq, handle.begin(), handle.end(),
    [&] ( const auto& eDrawable )
    {
      if ( drawablesView.contains(eDrawable) == false )
        return;

//      const auto&& [cGeometryBuffer, cTransform]
//          = drawablesView.get <const GeometryBuffer, const Transform> (eDrawable);

      auto& cTransform = drawablesView.get <const Transform> (eDrawable);

      const auto& handle = drawablesView.handle();
      const auto begin = handle.data();
      const auto index = &eDrawable - begin;

      worldMatrices[index] = GetWorldMatrix(registry, eDrawable, cTransform);
    });

  }

  if ( registry.ctx().contains <EditorRenderData> () == false )
    registry.ctx().emplace <EditorRenderData> ();

  auto& renderData = registry.ctx().get <EditorRenderData> ();

  auto& glProgram = registry.ctx().get <GlProgram> ();

  glProgram.use();

  glEnable(GL_SCISSOR_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const auto mainWindowSize = olc::renderer->ptrPGE->GetWindowSize();

  const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);
  const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);

  const auto uTransform = glProgram.uniformLocation("uTransform");
  const auto aPos = glProgram.attribLocation("aPos");
  const auto aUv = glProgram.attribLocation("aUv");
  const auto aColor = glProgram.attribLocation("aColor");

  const auto nullTexture = renderData.nullTexture.Decal()->id;
  const auto uvBufferFront = renderData.uvBufferFront;
  const auto uvBufferBack = renderData.uvBufferBack;
  const auto lineBuffer = renderData.lineBuffer;

//  immediate

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
    const auto viewProjection = cCamera->projMatrix() * camView;
    const auto camViewport = viewport.viewport;

    GLenum renderMode = GL_LINE_LOOP;

    if ( cCamera->renderMode == Camera::RenderMode::Solid )
    {
      glEnable(GL_DEPTH_TEST);
      renderMode = GL_TRIANGLE_FAN;
    }
    else
      glDisable(GL_DEPTH_TEST);

    glEnableVertexAttribArray(aPos);
    glEnableVertexAttribArray(aUv);

    glViewport(camViewport.x, mainWindowSize.y - camViewport.y - camViewport.w, camViewport.z, camViewport.w);
    glScissor(camViewport.x, mainWindowSize.y - camViewport.y - camViewport.w, camViewport.z, camViewport.w);

    auto drawablesView = const_registry.view <Transform, GeometryBuffer, TextureBuffer> ();

    const auto& handle = drawablesView.handle();

    const bool isWireframe =
      cCamera->renderMode != Camera::RenderMode::Solid ||
      cCamera->textureMode != Camera::TextureMode::Textured;

    glEnable(GL_CULL_FACE);

//    immediate

    glVertexAttrib4Nubv(aColor, (GLubyte*) &olc::WHITE.n);

    GLuint bufferIdPrev {};

    std::for_each(std::execution::unseq, handle.begin(), handle.end(),
    [&, cCamera = cCamera] ( const auto& eDrawable )
    {
      if ( drawablesView.contains(eDrawable) == false )
        return;

      const auto&& [cTransform, cGeometryBuffer, cTextureBuffer]
        = drawablesView.get <const Transform,const GeometryBuffer, const TextureBuffer> (eDrawable);

      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        return;

      const auto begin = drawablesView.handle().data();
      const auto index = &eDrawable - begin;

      const auto mvp = viewProjection * worldMatrices[index];
//      const auto mvp = viewProjection;

      glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(mvp));

      if ( gBuffer->id != bufferIdPrev )
      {
        bufferIdPrev = gBuffer->id;

        glBindBuffer(GL_ARRAY_BUFFER, gBuffer->id);
        glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, 0);
      }

      glBindBuffer(GL_ARRAY_BUFFER, uvBufferFront);
      glVertexAttribPointer(aUv, 2, GL_FLOAT, GL_FALSE, 0, 0);

      glCullFace(GL_BACK);
      glBindTexture(GL_TEXTURE_2D, nullTexture);

      if ( isWireframe == false &&
           cTextureBuffer.textures.size() != 0 )
      {
        TextureId textureId = cTextureBuffer.textures.at(0);

        auto texture = textures.get(textureId);

        if ( texture != nullptr &&
             texture->Decal() != nullptr )
        {
          glBindTexture(GL_TEXTURE_2D, texture->Decal()->id);
          glDrawArrays(renderMode, 0, gBuffer->vertices.size());
        }

        if ( cTextureBuffer.textures.size() < 2 )
          return;

        textureId = cTextureBuffer.textures.at(1);
        texture = textures.get(textureId);

        if ( texture != nullptr &&
             texture->Decal() != nullptr )
        {
          glCullFace(GL_FRONT);
          glBindTexture(GL_TEXTURE_2D, texture->Decal()->id);

          glBindBuffer(GL_ARRAY_BUFFER, uvBufferBack);
          glVertexAttribPointer(aUv, 2, GL_FLOAT, GL_FALSE, 0, 0);
        }
      }

      glDrawArrays(renderMode, 0, gBuffer->vertices.size());
    });

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, nullTexture);

    glDisableVertexAttribArray(aUv);

    const std::vector <glm::vec2> viewportOutline
    {
      {1.0f, camViewport.w},
      {1.0f, 1.0f},
      {camViewport.z - 0.0f, 1.0f},
      {camViewport.z - 0.0f, camViewport.w},
    };

    auto projection = glm::ortho(0.0f, camViewport.z, 0.0f, camViewport.w, -1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, viewportOutline.size() * sizeof(glm::vec2),
                 viewportOutline.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(projection));

    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttrib4Nubv(aColor, (GLubyte*) &colorViewport);

    glDrawArrays(GL_LINE_LOOP, 0, 4);


    const glm::vec2 windowPos
    {
      viewport.window.pos.x,
      viewport.window.pos.y,
    };

    const glm::vec2 windowSize
    {
      viewport.window.size.x,
      viewport.window.size.y,
    };

    glViewport(windowPos.x, mainWindowSize.y - windowPos.y - windowSize.y, windowSize.x, windowSize.y);
    glScissor(windowPos.x, mainWindowSize.y - windowPos.y - windowSize.y, windowSize.x, windowSize.y);

    const std::vector <glm::vec2> windowOutline
    {
      {1.0f, windowSize.y},
      {1.0f, 1.0f},
      {windowSize.x - 0.0f, 1.0f},
      {windowSize.x - 0.0f, windowSize.y},
    };

    projection = glm::ortho(0.0f, windowSize.x, 0.0f, windowSize.y, 0.0f, 1.0f);

    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    windowOutline.size() * sizeof(glm::vec2),
                    windowOutline.data());

    glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(projection));

    glVertexAttrib4Nubv(aColor, (GLubyte*) &colorWindow);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_BLEND);

  glProgram.unuse();
}

// delayed
void
EditorCullingSystem1(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::Transform;
  using compos::GeometryBuffer;
  using compos::TextureBuffer;
  using types::GlProgram;
  using types::GeometryAssetManager;
  using types::TextureAssetManager;
  using ui::ViewportManagerUi;

  struct alignas(64) Instance
  {
    glm::mat4 mvp;

    GLenum renderMode {GL_LINE_LOOP};
    GLuint buffer {0};
    GLuint texture {0};

    Instance() = default;
  };


  struct Batch
  {
    size_t length {};

    GLuint buffer {0};
    GLuint texture {0};
  };

  auto& glProgram = registry.ctx().get <GlProgram> ();

  glProgram.use();

  glEnable(GL_SCISSOR_TEST);
  glEnable(GL_DEPTH_TEST);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  const auto& const_registry = registry;

  auto& geometry = registry.ctx().get <GeometryAssetManager> ();
  auto& viewportManagerUi = const_registry.ctx().get <ViewportManagerUi> ();
  auto& textures  = registry.ctx().get <TextureAssetManager> ();

  const auto mainWindowSize = olc::renderer->ptrPGE->GetWindowSize();

  const uint32_t colorViewport = ImGui::GetColorU32(ImGuiCol_FrameBg);
  const uint32_t colorWindow = ImGui::GetColorU32(ImGuiCol_Border, 0.75f);

  const auto uniformPos = glProgram.uniformLocation("transform");
  const auto aPos = glProgram.attribLocation("aPos");
  const auto aUv = glProgram.attribLocation("aUV");
  const auto aColor = glProgram.attribLocation("aColor");

  olc::Renderable nullTexture;
  nullTexture.Create(1, 1);
  nullTexture.Sprite()->SetPixel({}, olc::WHITE);
  nullTexture.Decal()->Update();

  GLuint lineBuffer {};
  glGenBuffers(1, &lineBuffer);

  std::vector <glm::vec2> uv
  {
    {0.0f, 1.0f},
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {1.0f, 1.0f},
  };

  GLuint uvBuffer {};
  glGenBuffers(1, &uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);

  glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(decltype(uv)::value_type),
               uv.data(), GL_STATIC_DRAW);

  std::vector <glm::vec4> color
  {
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 0.0f, 1.0f},
  };

  GLuint colorBuffer {};
  glGenBuffers(1, &colorBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
  glBufferData(GL_ARRAY_BUFFER, color.size() * sizeof(decltype(color)::value_type),
               color.data(), GL_STATIC_DRAW);

  static std::vector <Instance> decals {};
  decals.resize(const_registry.storage <GeometryBuffer> ().size());

//  delayed

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
    const auto viewProjection = cCamera->projMatrix() * camView;
    const auto camViewport = viewport.viewport;

    const auto renderMode
      = cCamera->renderMode == Camera::RenderMode::Solid
      ? GL_TRIANGLE_FAN
      : GL_LINE_LOOP;

    std::atomic <size_t> i {};

    {
      auto drawablesView = const_registry.view <GeometryBuffer, Transform, TextureBuffer> ();

      const auto& handle = drawablesView.handle();
      const auto begin = handle.data();

      std::for_each(std::execution::unseq, handle.begin(), handle.end(),
      [&, cCamera = cCamera] ( const auto& eDrawable )
      {
        if ( drawablesView.contains(eDrawable) == false )
          return;

        const auto&& [cGeometryBuffer, cTransform, cTextureBuffer]
            = drawablesView.get <const GeometryBuffer, const Transform, const TextureBuffer> (eDrawable);

        const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

        if ( gBuffer == nullptr )
          return;

        auto& instance = decals[&eDrawable - begin];

//        static thread_local Instance instance;

        instance.mvp = viewProjection * GetWorldMatrix(registry, eDrawable, cTransform);
        instance.renderMode = renderMode;
        instance.buffer = gBuffer->id;
        instance.texture = nullTexture.Decal()->id;

        if (  cCamera->textureMode == Camera::TextureMode::Textured &&
              cTextureBuffer.textures.size() != 0 )
        {
          const TextureId textureId = cTextureBuffer.textures.at(0);

          const auto texture = textures.get(textureId);

          if ( texture != nullptr &&
               texture->Decal() != nullptr )
            instance.texture = texture->Decal()->id;
        }

//        decals[&eDrawable - begin] = instance;
        i.fetch_add(1, std::memory_order_relaxed);
      });
    }

    if ( cCamera->renderMode != Camera::RenderMode::Solid ||
         cCamera->textureMode != Camera::TextureMode::Textured )
    {
      auto drawablesView = const_registry.view <GeometryBuffer, Transform> (entt::exclude <TextureBuffer>);

      const auto& handle = drawablesView.handle();
      const auto begin = handle.data();

      std::for_each(std::execution::par_unseq, handle.begin(), handle.end(),
      [&, cCamera = cCamera] ( const auto& eDrawable )
      {
        if ( drawablesView.contains(eDrawable) == false )
          return;

        const auto&& [cGeometryBuffer, cTransform]
            = drawablesView.get <const GeometryBuffer, const Transform> (eDrawable);

        const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

        if ( gBuffer == nullptr )
          return;

        auto& instance = decals[&eDrawable - begin];

//        static thread_local Instance instance;

        instance.mvp = viewProjection * GetWorldMatrix(registry, eDrawable, cTransform);
        instance.renderMode = renderMode;
        instance.buffer = gBuffer->id;
        instance.texture = nullTexture.Decal()->id;

//        decals[&eDrawable - begin] = instance;
        i.fetch_add(1, std::memory_order_relaxed);
      });
    }

    glViewport(camViewport.x, mainWindowSize.y - camViewport.y - camViewport.w, camViewport.z, camViewport.w);
    glScissor(camViewport.x, mainWindowSize.y - camViewport.y - camViewport.w, camViewport.z, camViewport.w);

    glCullFace(GL_BACK);

    glEnableVertexAttribArray(aPos);
    glEnableVertexAttribArray(aUv);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(aUv, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttrib4Nubv(aColor, (GLubyte*) &olc::WHITE.n);

    for ( size_t iCurrent = 0; iCurrent < i; ++iCurrent )
    {
      const auto& instance = decals[iCurrent];

      glBindTexture(GL_TEXTURE_2D, instance.texture);

      glUniformMatrix4fv(uniformPos, 1, GL_FALSE, glm::value_ptr(instance.mvp));

      glBindBuffer(GL_ARRAY_BUFFER, instance.buffer);
      glVertexAttribPointer(aPos, 3, GL_FLOAT, GL_FALSE, 0, 0);

      glDrawArrays(instance.renderMode, 0, 4);
    }

    glDisable(GL_DEPTH_TEST);

    glBindTexture(GL_TEXTURE_2D, nullTexture.Decal()->id);

    glDisableVertexAttribArray(aUv);

    const std::vector <glm::vec2> viewportOutline
    {
      {1.0f, camViewport.w},
      {1.0f, 1.0f},
      {camViewport.z - 0.0f, 1.0f},
      {camViewport.z - 0.0f, camViewport.w},
    };

    auto projection = glm::ortho(0.0f, camViewport.z, 0.0f, camViewport.w, -1.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
    glBufferData(GL_ARRAY_BUFFER, viewportOutline.size() * sizeof(glm::vec2),
                 viewportOutline.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix4fv(uniformPos, 1, GL_FALSE, glm::value_ptr(projection));

    glVertexAttribPointer(aPos, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glVertexAttrib4Nubv(aColor, (GLubyte*) &colorViewport);

    glDrawArrays(GL_LINE_LOOP, 0, 4);


    const glm::vec2 windowPos
    {
      viewport.window.pos.x,
      viewport.window.pos.y,
    };

    const glm::vec2 windowSize
    {
      viewport.window.size.x,
      viewport.window.size.y,
    };

    glViewport(windowPos.x, mainWindowSize.y - windowPos.y - windowSize.y, windowSize.x, windowSize.y);
    glScissor(windowPos.x, mainWindowSize.y - windowPos.y - windowSize.y, windowSize.x, windowSize.y);

    const std::vector <glm::vec2> windowOutline
    {
      {1.0f, windowSize.y},
      {1.0f, 1.0f},
      {windowSize.x - 0.0f, 1.0f},
      {windowSize.x - 0.0f, windowSize.y},
    };

    projection = glm::ortho(0.0f, windowSize.x, 0.0f, windowSize.y, 0.0f, 1.0f);

    glBufferSubData(GL_ARRAY_BUFFER, 0,
                    windowOutline.size() * sizeof(glm::vec2),
                    windowOutline.data());

    glUniformMatrix4fv(uniformPos, 1, GL_FALSE, glm::value_ptr(projection));

    glVertexAttrib4Nubv(aColor, (GLubyte*) &colorWindow);

    glDrawArrays(GL_LINE_LOOP, 0, 4);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glDeleteBuffers(1, &colorBuffer);
  glDeleteBuffers(1, &uvBuffer);
  glDeleteBuffers(1, &lineBuffer);

  glDisableVertexAttribArray(aUv);
  glDisableVertexAttribArray(aColor);
  glDisableVertexAttribArray(aPos);

  glDisable(GL_SCISSOR_TEST);

  glUseProgram(0);
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

//      auto vb = buffer;
//      auto& tint = vb.tint.front();

//      tint = olc::PixelF( resultTint.r, resultTint.g,
//                          resultTint.b, resultTint.a );

//      cCamera.zBuffer.erase(iter);
//      iter = cCamera.zBuffer.emplace(vb, eLightTgt);
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

//  for ( const auto&& [eCamera, cCamera]
//          : const_registry.view <Camera> ().each() )
//  {
//    for ( const auto& [buffer, entity] : cCamera.zBuffer )
//    {
//      auto vertices = vec_to_array(buffer.vertices);

//      if ( cCamera.renderMode == Camera::RenderMode::Solid )
//      {
//        olc::Decal* decal {};

//        if ( cCamera.textureMode == Camera::TextureMode::Textured )
//        {
//          const TextureBuffer* textureBuffer = const_registry.try_get <TextureBuffer> (entity);
//          if ( textureBuffer != nullptr )
//          {
//            if ( (textureBuffer->textures.size() > 0 &&
//                  buffer.windingOrder == VertexBuffer::WindingOrder::CounterClockWise) ||
//                 (textureBuffer->textures.size() > 1 &&
//                  buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise) )
//            {
//              auto& textures  = registry.ctx().get <TextureAssetManager> ();
//              const TextureId textureId = textureBuffer->textures.at((int) buffer.windingOrder);

//              const auto texture = textures.get(textureId);
//              decal = texture ? texture->Decal() : nullptr;

//              if (  decal != nullptr &&
//                    buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise )
//                std::reverse(vertices.begin(), vertices.end());
//            }
//          }
//        }

//        if ( decal == nullptr )
//          continue;

//        auto tint = olc::WHITE;

//        if (  cCamera.lightingMode == Camera::LightingMode::Diffuse &&
//              registry.all_of <LightTarget> (entity) == true )
//            tint = buffer.tint.front();

//        else if ( auto cTextureTint = const_registry.try_get <TextureTint> (entity);
//                  cTextureTint != nullptr )
//          tint = cTextureTint->tint;

//        pge->DrawWarpedDecal( decal, vertices, tint );
//        continue;
//      }

//      pge->DrawPolyLineDecal(buffer.vertices, olc::GREY);
//    }
//  }
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

    for ( const auto& [vBuf, entity] : cCamera.zBuffer )
      if ( entity == eListener )
      {
//        olc::renderer->ptrPGE->DrawPolyLineDecal(vBuf.vertices, cListenerColor->color);
        break;
      }
  }
}

} // namespace cqde::systems
