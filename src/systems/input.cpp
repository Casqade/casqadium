#include <cqde/systems/input.hpp>

#include <cqde/assert.hpp>

#include <cqde/types/CasqadiumEngine.hpp>
#include <cqde/types/MouseCursor.hpp>
#include <cqde/types/assets/MouseCursorAssetManager.hpp>

#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/WantsMouseGrabbed.hpp>
#include <cqde/components/WantsCursorOverridden.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>


namespace cqde::systems
{

void
MouseGrabSystem(
  const entt::registry& registry )
{
  using namespace compos;
  using types::CasqadiumEngine;

  const auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  const auto mainWindow = engine.window();
  const auto hoveredWindow = engine.hoveredWindow();
  const auto focusedWindow = engine.focusedWindow();

  CQDE_ASSERT_DEBUG(mainWindow != nullptr, return);

  const bool mouseGrabActive =
    glfwGetInputMode(mainWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;

  const auto setCursorMode =
  [mainWindow, hoveredWindow, focusedWindow]
  ( const int32_t cursorMode )
  {
    if ( hoveredWindow != nullptr )
      glfwSetInputMode(hoveredWindow, GLFW_CURSOR, cursorMode);

    if ( focusedWindow != nullptr )
      glfwSetInputMode(focusedWindow, GLFW_CURSOR, cursorMode);

    glfwSetInputMode(mainWindow, GLFW_CURSOR, cursorMode);
  };

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    for ( const auto&& [entity]
            : registry.view <SubscriberInput, SubscriberUpdate, WantsMouseGrabbed> ().each() )
    {
      if ( mouseGrabActive == true )
        return;

      else
        return setCursorMode(GLFW_CURSOR_DISABLED);
    }

  else
    for ( const auto&& [entity]
            : registry.view < SubscriberInput, SubscriberUpdate, WantsMouseGrabbed,
                              CasqadiumEditorInternal> ().each() )
    {
      if ( mouseGrabActive == true )
        return;

      else
        return setCursorMode(GLFW_CURSOR_DISABLED);
    }

  if ( mouseGrabActive == true )
    setCursorMode(GLFW_CURSOR_NORMAL);
}

void
MouseCursorOverrideSystem(
  entt::registry& registry )
{
  using namespace compos;
  using types::CasqadiumEngine;
  using types::MouseCursorAssetManager;

  const auto& engine = *registry.ctx().get <CasqadiumEngine*> ();

  const auto mainWindow = engine.window();
  const auto hoveredWindow = engine.hoveredWindow();
  const auto focusedWindow = engine.focusedWindow();

  CQDE_ASSERT_DEBUG(mainWindow != nullptr, return);

  if ( glfwGetInputMode(mainWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED )
    return; // mouse grab has higher priority

  const auto setCustomCursor =
  [mainWindow, hoveredWindow, focusedWindow]
  ( GLFWcursor* cursor )
  {
    if ( hoveredWindow != nullptr )
      glfwSetCursor(hoveredWindow, cursor);

    if ( focusedWindow != nullptr )
      glfwSetCursor(focusedWindow, cursor);

    glfwSetCursor(mainWindow, cursor);
  };

  auto& cursorManager = registry.ctx().get <MouseCursorAssetManager> ();

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    for ( const auto&& [entity, cCursorOverrider]
            : registry.view <WantsCursorOverridden, SubscriberUpdate> ().each() )
    {
      const auto cursor = cursorManager.get(cCursorOverrider.cursor);

      if ( cursor == nullptr )
        break;

      return setCustomCursor(*cursor);
    }

  else
    for ( const auto&& [entity, cCursorOverrider]
            : registry.view < WantsCursorOverridden, SubscriberUpdate,
                              CasqadiumEditorInternal> ().each() )
    {
      const auto cursor = cursorManager.get(cCursorOverrider.cursor);

      if ( cursor == nullptr )
        break;

      return setCustomCursor(*cursor);
    }

  setCustomCursor(NULL);
}

} // namespace cqde::systems
