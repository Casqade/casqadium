#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityMetaInfo.hpp>

#include <cqde/common.hpp>
#include <cqde/ecs_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <entt/entity/registry.hpp>
#include <entt/meta/resolve.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>


namespace cqde::ui
{

EntityManagerUi::EntityManagerUi(
  types::EntityManager* entityMgr )
  : mSelectedEntity{entt::null}
  , mSelectedComponent{entt::null}
  , mEntityMgr{entityMgr}
{}

void
EntityManagerUi::ui_show(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::EntityMetaInfo;

  CQDE_ASSERT_DEBUG(mEntityMgr != nullptr, return);

  if ( ImGui::Begin("Registry view", nullptr,
                    ImGuiWindowFlags_HorizontalScrollbar ) == false )
  {
    ImGui::End(); // Registry view
    return;
  }

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
    mRegistryFilter.show(registry);

  ImGui::Separator();

  const auto tagType = mEntityMgr->componentType("Tag");

  const bool entityNameInvalid =  mNewEntityName.empty() == true ||
                                  mNewEntityName == null_id.str() ||
                                  mEntityMgr->get(mNewEntityName) != entt::null;

  ImGui::BeginDisabled(entityNameInvalid);

  const bool newEntityAdded = ImGui::Button("+##entityAdd");

  ImGui::EndDisabled();

  ImGui::SameLine();

  if ( ImGui::Button("%##entityIdGen") )
    mNewEntityName = mEntityMgr->idGenerate(mNewEntityName).str();

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Generate unique entity ID");

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newEntityId", "New entity ID", &mNewEntityName,
                           ImGuiInputTextFlags_AutoSelectAll);

  if ( newEntityAdded == true )
  {
    const auto newEntity = mEntityMgr->entityCreate(mNewEntityName, registry);

    auto& cEntityInfo = registry.emplace <EntityMetaInfo> (newEntity);
    cEntityInfo.packageId = mRegistryFilter.package();
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollY |
                          ImGuiTableFlags_RowBg;

  if ( ImGui::BeginTable( "Entities", 1, tableFlags,
                          {0.0f, ImGui::GetContentRegionAvail().y}) )
  {
    ImGui::TableNextColumn();

    registry.each(
    [ this, &registry, tagType] ( const entt::entity entity )
    {
      const auto packageSelected = mRegistryFilter.package();

      if (  packageSelected.str().empty() == false &&
            packageSelected != registry.get <EntityMetaInfo> (entity).packageId )
        return;

      const auto entityId = registry.get <Tag> (entity).id;

      if ( mRegistryFilter.query(entityId) == false )
        return;

      bool componentFound {};

      each_component(entity, registry,
      [this, &componentFound] ( const ComponentType component )
      {
        componentFound = mRegistryFilter.query(component);

        if ( componentFound == true )
          return false;

        return true;
      });

      if ( componentFound == false )
        return;

      ImGui::PushID(entityId.str().c_str());

      if ( ImGui::SmallButton("-##entityRemove") )
        mEntityMgr->removeLater(entity);

      ImGui::PopID(); // entityId

      auto nodeFlags =  ImGuiTreeNodeFlags_OpenOnArrow |
                        ImGuiTreeNodeFlags_AllowItemOverlap |
                        ImGuiTreeNodeFlags_OpenOnDoubleClick;

      if ( entity == mSelectedEntity )
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

      ImGui::SameLine();

      const bool nodeOpened = ImGui::TreeNodeEx(entityId.str().c_str(), nodeFlags);

      if ( ImGui::IsItemActivated() == true &&
           ImGui::IsItemToggledOpen() == false )
      {
        if ( entity != mSelectedEntity )
          mSelectedComponent = entt::null;

        mSelectedEntity = entity;
      }

      if ( nodeOpened == true )
      {
        each_component(entity, registry,
        [ this, &registry, entity,
          entityId, tagType] ( const ComponentType componentType )
        {
          ImGui::PushID(componentType);

          ImGui::BeginDisabled(componentType == tagType);

          if ( ImGui::SmallButton("-##componentRemove") )
          {
            mEntityMgr->removeLater(entity, componentType);

            if ( mSelectedEntity == entity &&
                 mSelectedComponent == componentType )
              mSelectedComponent = entt::null;
          }

          ImGui::EndDisabled();

          auto flags = ImGuiTreeNodeFlags_Leaf |
                       ImGuiTreeNodeFlags_AllowItemOverlap |
                       ImGuiTreeNodeFlags_NoTreePushOnOpen;

          if ( mSelectedEntity == entity &&
               mSelectedComponent == componentType )
            flags |= ImGuiTreeNodeFlags_Selected;

          ImGui::SameLine();

          const bool selected = mSelectedEntity == entity &&
                                mSelectedComponent == componentType;

          if ( ImGui::Selectable(component_name(componentType).c_str(), selected) )
          {
            mSelectedEntity = entity;
            mSelectedComponent = componentType;
          }

          ImGui::PopID(); // componentType

          return true;
        });

        ImGui::Spacing();

        if ( ImGui::SmallButton("+##componentAdd") )
          ImGui::OpenPopup("##componentAddPopup");

        if ( ImGui::IsItemHovered() )
          ImGui::SetTooltip("Add component");

        ImGui::Spacing();

        if ( ImGui::BeginPopup("##componentAddPopup") )
        {
          if ( ImGui::IsWindowAppearing() )
            ImGui::SetKeyboardFocusHere(2);

          mNewComponentFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

          bool componentsFound {};

          for ( const auto& componentName : mEntityMgr->componentNames() )
          {
            if ( mNewComponentFilter.query(componentName) == false )
              continue;

            componentsFound = true;

            if ( ImGui::Selectable(componentName.c_str(), false) )
            {
              mEntityMgr->componentAdd(mEntityMgr->componentType(componentName),
                                       entity, registry);

              ImGui::CloseCurrentPopup();
              break;
            }
          }

          if ( componentsFound == false )
            ImGui::Text("No components matching filter");

          ImGui::EndPopup(); // componentAddPopup
        }

        ImGui::TreePop(); // entityId
      }
    });

    ImGui::EndTable(); // Entities
  }

  ImGui::End(); // Registry view

  ui_show_component_window(registry);

  ui_show_scene_graph_window(registry);
}

void
EntityManagerUi::ui_show_component_window(
  entt::registry& registry )
{
  using compos::Tag;

  if ( ImGui::Begin("Component view") == false )
  {
    ImGui::End(); // Component view
    return;
  }

  if ( mSelectedEntity == entt::null ||
       mSelectedComponent == entt::null ||
       registry.valid(mSelectedEntity) == false )
  {
    ImGui::Text("No component selected");

    ImGui::End(); // Component view
    return;
  }

  const auto entityId = registry.get <Tag> (mSelectedEntity).id;

  auto component = entt::resolve(mSelectedComponent);

  if ( !component )
  {
    ImGui::Text("Selected component is invalid");

    ImGui::End(); // Component view
    return;
  }

  using fmt::format;
  using namespace entt::literals;

  const auto componentName = component_name(mSelectedComponent);

  ImGui::BulletText("%s", format("{} -> {}", entityId.str(), componentName).c_str());

  if ( component_is_empty(mSelectedComponent) == true )
  {
    ImGui::Text("No data for this component");
    ImGui::End(); // Component view
    return;
  }

  auto componentGet = component.func("get"_hs);
  auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), mSelectedEntity);

  if ( componentInstance )
  {
    auto componentUiShow = component.func("ui_edit_props"_hs);
    componentUiShow.invoke(componentInstance, mSelectedEntity, entt::forward_as_meta(registry));
  }

  ImGui::End(); // Component view
}

void
EntityManagerUi::ui_show_scene_graph_window(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::SceneNode;

  if ( ImGui::Begin("Scene graph view", NULL, ImGuiWindowFlags_HorizontalScrollbar) == false )
    return ImGui::End(); // SceneGraph view

  std::pair <entt::entity, entt::entity> nodeToAttach {entt::null, entt::null};
  std::pair <entt::entity, entt::entity> nodeToDestroy {entt::null, entt::null};

  const auto window = ImGui::GetCurrentWindow();

  if ( ImGui::BeginDragDropTargetCustom(window->WorkRect, window->ID) )
  {
    auto payload = ImGui::AcceptDragDropPayload("sceneNodePayload");

    if ( payload != nullptr )
    {
      IM_ASSERT(payload->DataSize == sizeof(entt::entity));

      const auto eDragged = *(const entt::entity*) payload->Data;

      nodeToAttach = {entt::null, eDragged};
    }
    ImGui::EndDragDropTarget();
  }

  const std::function <void(const entt::entity)> each_node =
  [&] ( const entt::entity eParent ) -> void
  {
    if ( eParent == entt::null )
      return;

    auto [cNode, cTag] = registry.try_get <SceneNode, Tag> (eParent);

    if ( cNode == nullptr ||
         cTag == nullptr )
      return;

    const auto nodeId = cTag->id;

    ImGui::PushID(nodeId.str().c_str());

    if ( ImGui::SmallButton("-##nodeDestroy") )
      nodeToDestroy = {cNode->parent.get_if_valid(registry), eParent};

    auto flags =  ImGuiTreeNodeFlags_OpenOnArrow |
                  ImGuiTreeNodeFlags_OpenOnDoubleClick |
                  ImGuiTreeNodeFlags_AllowItemOverlap;

    if ( cNode->children.empty() == true )
    {
      flags |= ImGuiTreeNodeFlags_Bullet;
      flags |= ImGuiTreeNodeFlags_Leaf;
      flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    if ( mSelectedEntity == eParent )
      flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::SameLine();
    const bool nodeOpened = ImGui::TreeNodeEx(nodeId.str().c_str(), flags);

    if ( ImGui::IsItemActivated() == true &&
         ImGui::IsItemToggledOpen() == false )
    {
      if ( eParent != mSelectedEntity )
        mSelectedComponent = entt::null;

      mSelectedEntity = eParent;
    }

    if ( ImGui::BeginDragDropSource() )
    {
      ImGui::SetDragDropPayload("sceneNodePayload",
                                &eParent, sizeof(eParent));
      ImGui::EndDragDropSource();
    }

    auto payload = ImGui::GetDragDropPayload();

    if ( payload != nullptr &&
         payload->IsDataType("sceneNodePayload") == true )
    {
      IM_ASSERT(payload->DataSize == sizeof(eParent));

      const auto eDragged = *(const entt::entity*) payload->Data;

      auto& cTagDragged = registry.get <Tag> (eDragged);

      if ( CanAddChildNode(registry, eParent, cTagDragged.id) == true &&
           ImGui::BeginDragDropTarget() )
      {
        if ( ImGui::AcceptDragDropPayload("sceneNodePayload") != nullptr )
          nodeToAttach = {eParent, eDragged};
        ImGui::EndDragDropTarget();
      }
    }

    if ( nodeOpened == false )
      return ImGui::PopID(); // nodeId

    for ( const auto& childRef :  (cNode->children) )
      each_node( mEntityMgr->get_if_valid(childRef.id, registry) );

    if ( cNode->children.empty() == false )
      ImGui::TreePop(); // cTag.id

    ImGui::PopID(); // nodeId
  };

  for ( auto&& [eNode, cNode, cTag] : registry.view <SceneNode, Tag> ().each() )
  {
    if ( cNode.parent.id != null_id )
      continue;

    each_node(eNode);
  }

  if ( nodeToAttach.second != entt::null )
  AttachChildNode(registry, nodeToAttach.first, nodeToAttach.second);

  if ( nodeToDestroy.second != entt::null )
    DestroyChildNode(registry, nodeToDestroy.first, nodeToDestroy.second);

  ImGui::End(); // SceneGraph view
}

void
EntityManagerUi::entitySelect(
  const entt::entity entity )
{
  mSelectedEntity = entity;
  mSelectedComponent = entt::null;
}

void
EntityManagerUi::componentSelect(
  const ComponentType component )
{
  mSelectedComponent = component;
}

entt::entity
EntityManagerUi::selectedEntity() const
{
  return mSelectedEntity;
}

ComponentType
EntityManagerUi::selectedComponent() const
{
  return mSelectedComponent;
}

} // namespace cqde::types
