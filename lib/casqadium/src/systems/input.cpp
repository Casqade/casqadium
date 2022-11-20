#include <cqde/systems/input.hpp>

#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/WantsMouseCentered.hpp>
#include <cqde/components/WantsMouseHidden.hpp>

#include <entt/entity/registry.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>


namespace cqde::systems
{

void
MouseCenteringSystem(
  const entt::registry& registry )
{
  using namespace compos;

  if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
    for ( const auto&& [entity]
            : registry.view <SubscriberInput, SubscriberUpdate, WantsMouseCentered> ().each() )
    {
      if ( olc::platform->ptrPGE->GetKeepMouseCentered() == false )
        olc::platform->ptrPGE->SetKeepMouseCentered(true);

      return;
    }

  else
    for ( const auto&& [entity]
            : registry.view < SubscriberInput, SubscriberUpdate,
                              WantsMouseCentered, CasqadiumEditorInternal> ().each() )
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
    for ( const auto&& [entity]
            : registry.view <SubscriberInput, SubscriberUpdate, WantsMouseHidden> ().each() )
      return olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});

  else
    for ( const auto&& [entity]
            : registry.view < SubscriberInput, SubscriberUpdate,
                              WantsMouseHidden, CasqadiumEditorInternal> ().each() )
      return olc::platform->ptrPGE->SetMouseCursor(olc::Mouse::Cursor{});

  olc::platform->ptrPGE->ResetMouseCursor();
}

} // namespace cqde::systems
