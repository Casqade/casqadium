#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/systems.hpp>

#include <cqde/types/SystemManager.hpp>


namespace cqde::types
{

void
CasqadiumEngine::registerSystems()
{
  using Phase = System::Phase;
  using namespace cqde::systems;

  auto& systemManager = mRegistry.ctx().get <SystemManager> ();

  systemManager.Register("CasqadiumEditorSystem",
                         EditorSystem,
                         Phase{Phase::Logic | Phase::Editor});

  systemManager.Register("CasqadiumEditorMouseGrabSystem",
                         MouseGrabSystem,
                         Phase{Phase::Logic | Phase::Editor});

  systemManager.Register("InteractionResetSystem",
                         InteractionResetSystem,
                         Phase::Logic);

  systemManager.Register("PhysicsSystem",
                         PhysicsSystem,
                         Phase::Logic);

  systemManager.Register("SequenceSystem",
                         SequenceSystem,
                         Phase::Logic);

  systemManager.Register("Audio3dSystem",
                         Audio3dSystem,
                         Phase::Logic);

  systemManager.Register("AudioDrivenTransformSystem",
                         AudioDrivenTransformSystem,
                         Phase::Logic);

  systemManager.Register("InteractionQuerySystem",
                         InteractionQuerySystem,
                         Phase::Logic);

  systemManager.Register("MouseGrabSystem",
                         MouseGrabSystem,
                         Phase::Logic);

  systemManager.Register("MouseCursorOverrideSystem",
                         MouseCursorOverrideSystem,
                         Phase::Logic);


  systemManager.Register("RenderBufferClearSystem",
                         RenderBufferClearSystem,
                         Phase::Render);

  systemManager.Register("CasqadiumEditorRenderBufferClearSystem",
                         RenderBufferClearSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("CasqadiumEditorRenderSystem",
                         EditorRenderSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("RenderSystem",
                         RenderSystem,
                         Phase::Render);

  systemManager.Register("PhysicsDebugRenderSystem",
                         PhysicsDebugRenderSystem,
                         Phase::Render);


  systemManager.Register("CasqadiumEditorRenderSystem",
                         RenderSystem,
                         Phase{Phase::Render | Phase::Editor});

  systemManager.Register("EditorPhysicsDebugRenderSystem",
                         EditorPhysicsDebugRenderSystem,
                         Phase::Render);

  systemManager.Register("InteractionHighlightSystem",
                         InteractionHighlightSystem,
                         Phase::Render);

  systemManager.Register("CasqadiumEditorEntityHighlightSystem",
                         EditorEntityHighlightSystem,
                         Phase{Phase::Render | Phase::Editor});
}

} // namespace cqde::types
