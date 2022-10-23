#include <cqde/components/physics/CollisionBody.hpp>

#include <cqde/types/physics/ColliderFactory.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/CollisionBody.h>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
CollisionBody::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::ColliderFactory;

  if ( ImGui::CollapsingHeader("State", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    bool active {};
    bool activePrev {};

    if ( body == nullptr )
      active = bodyState["active"].asBool();
    else
      active = body->isActive();

    activePrev = active;

    ImGui::Checkbox("Is active", &active);

    if ( active != activePrev )
    {
      if ( body == nullptr )
        bodyState["active"] = active;
      else
        body->setIsActive(active);
    }
  }

  if ( ImGui::CollapsingHeader("Colliders", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static ui::StringFilter shapeFilter {"Shape ID"};

  static bool colliderWindowOpened {};
  static int32_t selectedCollider = {-1};

  if ( ImGui::SmallButton("+##colliderAdd") )
    ImGui::OpenPopup("##colliderAddPopup");

  if ( ImGui::BeginPopup("##colliderAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    shapeFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    const auto& colliderFactory = registry.ctx().at <ColliderFactory> ();

    for ( const auto& shapeId : colliderFactory.colliders() )
    {
      if ( shapeFilter.query(shapeId) == false )
        continue;

      if ( ImGui::Selectable(shapeId.c_str(), false) )
      {
        colliders.push_back(colliderFactory.create(shapeId));
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    ImGui::EndPopup(); // colliderAddPopup
  }

  ImGui::Separator();

  for ( auto iter = colliders.begin();
        iter < colliders.end();
        ++iter )
  {
    const auto index = std::distance(colliders.begin(), iter);
    ImGui::PushID(index);

    if ( ImGui::SmallButton("-##colliderRemove") )
    {
      if ( selectedCollider >= index )
        selectedCollider = -1;

      iter = colliders.erase(iter);

      if ( iter == colliders.end() )
      {
        ImGui::PopID(); // index
        break;
      }
    }

    const auto shapeId = (*iter)->type();

    const bool selected = index == selectedCollider;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable(shapeId.c_str(),
                           selected, flags) )
    {
      selectedCollider = index;

      colliderWindowOpened = true;
      ImGui::SetWindowFocus("###colliderEditWindow");
    }

    static auto iter_dragged = colliders.end();

    if ( ImGui::IsItemHovered() == true )
      ImGui::SetTooltip("Drag to reorder");

    if ( ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) == true &&
         ImGui::IsMouseDown(ImGuiMouseButton_Left) == true )
    {
      auto mouseClickPos = ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left];

      if (  mouseClickPos.x > ImGui::GetItemRectMin().x &&
            mouseClickPos.x < ImGui::GetItemRectMax().x )
      {
        if ( iter_dragged != iter &&
             iter_dragged >= colliders.begin() &&
             iter_dragged < colliders.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged < colliders.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = colliders.end();

    ImGui::PopID(); // index
  }

  if ( colliderWindowOpened == false )
    return;

  if ( selectedCollider < 0 ||
       selectedCollider >= colliders.size() )
    return;

  const auto collider = colliders.at(selectedCollider);

  const auto windowTitle = format("Collider '{}'###colliderEditWindow",
                                  collider->type());

  if ( ImGui::Begin(windowTitle.c_str(),
                    &colliderWindowOpened, ImGuiWindowFlags_MenuBar) )
    collider->ui_show(registry);

  ImGui::End(); // windowTitle
}

} // namespace cqde
