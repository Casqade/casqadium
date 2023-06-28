#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>

#include <cqde/types/ui/widgets/IdSelector.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/assert.hpp>

#include <imgui.h>


namespace cqde::compos
{

void
SceneNode::ui_edit_props(
  const entt::entity entity,
  entt::registry& registry )
{
  using types::EntityManager;

  static ui::StringFilter entityFilter {"Entity ID"};

  auto& entityManager = registry.ctx().get <EntityManager> ();

  const auto id = registry.get <Tag> (entity).id;

  if ( ImGui::CollapsingHeader("Parent", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( ImGui::BeginCombo("##parentIdCombo", parent.id.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      entityFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      for ( const auto& entityId : entityManager.entities() )
      {
        if ( entityId == id )
          continue;

        if ( entityFilter.query(entityId.str()) == false )
          continue;

        if ( children.count(entityId) > 0 )
          continue;

        entt::entity eNode {entt::null};

        CQDE_ASSERT_DEBUG( entityId != null_id, continue );

        eNode = entityManager.get_if_valid(entityId, registry);

        CQDE_ASSERT_DEBUG( eNode != entt::null, continue );

        if ( registry.all_of <SceneNode> (eNode) == false )
          continue;

        if ( CanAddChildNode(registry, eNode, id) == false )
          continue;

        const bool selected = entityId == parent.id;

        if ( ImGui::Selectable(entityId.str().c_str(), selected) )
        {
          AttachChildNode(registry, eNode, entity);
          break;
        }
      }

      if (  entityFilter.query(null_id.str()) == true &&
            ImGui::Selectable(null_id.str().c_str(), null_id == parent.id) )
        AttachChildNode(registry, entt::null, entity);

      ImGui::EndCombo(); // parentIdCombo
    }
  }

  if ( ImGui::CollapsingHeader("Children", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    if ( children.empty() == true )
      ImGui::Text("None");

    else if ( ImGui::Button("Clear##childrenClear") )
      children.clear();;

    std::vector <types::EntityReference> childrenToRemove {};

    for ( const auto& child : children )
    {
      ImGui::PushID(child.id.str().c_str());

      ImGui::BulletText("%s", child.id.str().c_str());

      ImGui::SameLine();
      if ( ImGui::SmallButton("-##childRemove") )
        childrenToRemove.push_back(child);

      ImGui::PopID(); // child.id
    }

    for ( const auto& child : childrenToRemove )
    {
      DetachChildNode(registry, entity, child.get(registry));
      children.erase(child);
    }


    const auto childAddPopupLabel {"##childAddPopup"};

    static ui::IdSelector entitySelector {
      "Entity ID", childAddPopupLabel };

    if ( ImGui::SmallButton("+##childAdd") )
      ImGui::OpenPopup(childAddPopupLabel);

    entitySelector.selectPopup(
      entityManager.entities(),
      [&registry, &entityManager, entity]
      ( const auto& entityId )
      {
        const auto eNode = entityManager.get_if_valid(
          entityId, registry );

        AttachChildNode(registry, entity, eNode);
      },
      [&registry, &entityManager, &id, &children = children, entity]
      ( const auto& entityId )
      {
        if (  entityId == id ||
              entityId == null_id ||
              children.count(entityId) > 0 )
          return false;

        const auto eNode = entityManager.get_if_valid(
          entityId, registry );

        CQDE_ASSERT_DEBUG( eNode != entt::null, return false);

        if ( registry.all_of <SceneNode> (eNode) == false )
          return false;

        return CanAddChildNode(registry, entity, entityId);
      });
  }
}

} // namespace cqde::compos
