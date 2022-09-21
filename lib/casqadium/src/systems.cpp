#include <cqde/systems.hpp>

#include <cqde/callbacks.hpp>
#include <cqde/render_helpers.hpp>

#include <cqde/types/assets/TextureAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/types/ui/AssetManagerUi.hpp>
#include <cqde/types/ui/CallbackManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/ui/PackageManagerUi.hpp>
#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/SystemManagerUi.hpp>
#include <cqde/types/ui/ViewportManagerUi.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>


namespace cqde::systems
{

void
EditorSystem(
  entt::registry& registry )
{
  using namespace compos;
  using namespace cqde::ui;
  using types::InputManager;
  using types::EntityManager;
  using types::InputBindingRelative;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
  ImGui::DockSpaceOverViewport(NULL, ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::PopStyleColor();

  auto& entityManager = registry.ctx().at <EntityManager> ();

  callbacks::editorBindingsAssign(registry);
  callbacks::editorCameraCreate(registry);
  callbacks::editorControllerCreate(registry);

  registry.ctx().at <PackageManagerUi> ().ui_show(registry);
  registry.ctx().at <AssetManagerUi> ().ui_show(registry);
  registry.ctx().at <CallbackManagerUi> ().ui_show(registry);
  registry.ctx().at <PrefabManagerUi> ().ui_show(registry);
  registry.ctx().at <EntityManagerUi> ().ui_show(registry);
  registry.ctx().at <InputManagerUi> ().ui_show(registry);
  registry.ctx().at <SystemManagerUi> ().ui_show(registry);
  registry.ctx().at <ViewportManagerUi> ().ui_show(registry);
}

void
EditorEntityHighlightSystem(
  entt::registry& registry )
{
  using compos::Camera;
  using compos::CasqadiumEditorInternal;
  using ui::EntityManagerUi;
  using types::EntityManager;

  const auto& entityManager   = registry.ctx().at <EntityManager> ();
  const auto& entityManagerUi = registry.ctx().at <EntityManagerUi> ();

  for ( const auto selectedEntity : entityManagerUi.selectedEntities() )
  {
    if ( selectedEntity == entt::null )
      return;

    for ( const auto&& [eCamera, cCamera] : registry.view <Camera, CasqadiumEditorInternal> ().each() )
      for ( const auto& [vBuf, entity] : cCamera.zBuffer )
        if ( entity == selectedEntity )
          drawLines(vBuf.vertices, olc::YELLOW, LineRenderMode::Loop);
  }
};

void
MouseCenteringSystem(
  entt::registry& registry )
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
  entt::registry& registry )
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

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform> ().each() )
  {
    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewportScaled();

    for ( const auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
            : registry.view <GeometryBuffer, SceneNode, Transform>().each() )
    {
      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView * GetWorldMatrix(registry, cTransform, cNode);

      const auto vBuffer = vertexShader(
        *gBuffer,
        modelView,
        camProjection,
        camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      cCamera.zBuffer.emplace( vBuffer, eDrawable );
    }
  }
}

std::array <olc::vf2d, 4>
vec_to_array( const std::vector <olc::vf2d>& src )
{
  std::array <olc::vf2d, 4> dest;
  std::copy( src.begin(), src.begin() + 4, dest.begin() );

  return dest;
}

void
RenderSystem( entt::registry& registry )
{
  using namespace compos;
  using namespace types;

  registry.sort <Camera> (
  [] ( const Camera& lhs, const Camera& rhs )
  {
    return lhs.layer > rhs.layer;
  });

  for ( const auto&& [eCamera, cCamera] : registry.view <Camera> ().each() )
  {
    for ( const auto& [buffer, entity] : cCamera.zBuffer )
    {
      auto vertices = vec_to_array(buffer.vertices);

      if ( cCamera.renderMode == Camera::RenderMode::Solid )
      {
        if ( cCamera.textureMode == Camera::TextureMode::Textured )
        {
          const TextureBuffer* textureBuffer = registry.try_get <TextureBuffer> (entity);
          if ( textureBuffer != nullptr )
          {
            if ( (textureBuffer->textures.size() > 0 &&
                  buffer.windingOrder == VertexBuffer::WindingOrder::CounterClockWise) ||
                 (textureBuffer->textures.size() > 1 &&
                  buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise) )
            {
              auto& textures  = registry.ctx().at <TextureAssetManager> ();
              const TextureId textureId = textureBuffer->textures.at((int) buffer.windingOrder);

              const auto  texture = textures.get(textureId);
              olc::Decal* decal = texture ?
                                  texture->Decal() : nullptr;

              if ( decal != nullptr )
              {
                if ( buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise )
                  std::reverse(vertices.begin(), vertices.end());

                olc::renderer->ptrPGE->DrawWarpedDecal( decal, vertices );
              }
            }
          }
        }
      }
      else
        drawLines(buffer.vertices, olc::GREY, LineRenderMode::Loop);
    }
  }
}

void
RenderBufferClearSystem( entt::registry& registry )
{
  using compos::Camera;

  for ( const auto&& [eCamera, cCamera] : registry.view <Camera> ().each() )
    cCamera.zBuffer.clear();
}

} // namespace cqde::systems
