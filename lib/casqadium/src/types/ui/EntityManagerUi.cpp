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
  CQDE_ASSERT_DEBUG(mEntityMgr != nullptr, return);

  if ( ImGui::Begin("Registry view") == false )
  {
    ImGui::End(); // Registry view
    return;
  }

  ui_show_filter_section(registry);

  const auto tableFlags = ImGuiTableFlags_Resizable;

  if ( ImGui::BeginTable("RegistryTable", 2, tableFlags,
                         ImGui::GetContentRegionAvail()) )
  {
    ImGui::TableNextColumn();
    ui_show_entities_table(registry);

    ImGui::TableNextColumn();
    ui_show_nodes_table(registry);

    ImGui::EndTable(); // RegistryTable
  }

  ImGui::End(); // Registry view

  ui_show_component_window(registry);
}

void
EntityManagerUi::ui_show_component_window(
  entt::registry& registry )
{
  using fmt::format;
  using namespace entt::literals;

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

  const auto componentName = component_name(mSelectedComponent);

  ImGui::BulletText("%s", format("{} -> {}", entityId.str(), componentName).c_str());

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

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
EntityManagerUi::ui_show_filter_section(
  entt::registry& registry )
{
  using compos::Tag;
  using compos::SceneNode;
  using compos::EntityMetaInfo;

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
    mRegistryFilter.show(registry);

  ImGui::Separator();

  if ( ImGui::Button("+##entityAdd") )
  {
    if ( mNewEntityName.empty() == true ||
         mNewEntityName == null_id.str() ||
         mEntityMgr->get(mNewEntityName) != entt::null )
      mNewEntityName = mEntityMgr->idGenerate(mNewEntityName).str();

    const auto newEntity = mEntityMgr->entityCreate(mNewEntityName, registry);

    auto& cEntityInfo = registry.emplace <EntityMetaInfo> (newEntity);
    cEntityInfo.packageId = mRegistryFilter.package();
  }

  ImGui::SameLine();

  ImGui::BeginDisabled( mClipboard["type"] != "entities" ||
                        mClipboard["payload"].empty() );

  if ( ImGui::Button("Paste##entityPaste") )
    prefabDeserialize(registry, mClipboard["payload"]);

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newEntityId", "New entity ID", &mNewEntityName,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();
}

void
EntityManagerUi::ui_show_entities_table(
  entt::registry& registry )
{
  using fmt::format;
  using compos::Tag;
  using compos::SceneNode;
  using compos::EntityMetaInfo;

  const auto window = ImGui::GetCurrentWindow();

  if ( ImGui::BeginDragDropTargetCustom(window->WorkRect, window->ID) )
  {
    const auto payload = ImGui::AcceptDragDropPayload("prefabPayload");

    if ( payload != nullptr )
    {
      IM_ASSERT(payload->DataSize == sizeof(Json::Value));

      const auto prefab = *(const Json::Value*) payload->Data;

      prefabDeserialize(registry, prefab);
    }
    ImGui::EndDragDropTarget();
  }

  const auto tagType = mEntityMgr->componentType <Tag> ();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable("EntitiesTable", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  registry.sort <Tag> (
  [] ( const Tag& lhs, const Tag& rhs )
  {
    return lhs.id < rhs.id;
  });

  registry.view <Tag> ().each(
  [ this, &registry, tagType] ( const entt::entity entity,
                                const Tag& cTag )
  {
    if ( mRegistryFilter.query(registry, entity) == false )
      return;

    const auto entityId = cTag.id.str();

    ImGui::PushID(entityId.c_str());

    if ( ImGui::SmallButton("-##entityRemove") )
    {
      mEntityMgr->removeLater(entity);

      if ( entity == mSelectedEntity )
        entityDeselect();
    }

    ImGui::PopID(); // entityId

    auto nodeFlags =  ImGuiTreeNodeFlags_OpenOnArrow |
                      ImGuiTreeNodeFlags_AllowItemOverlap |
                      ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if ( entity == mSelectedEntity )
      nodeFlags |= ImGuiTreeNodeFlags_Selected;

    ImGui::SameLine();

    const bool nodeOpened = ImGui::TreeNodeEx(entityId.c_str(), nodeFlags);

    ImGui::PushID(entityId.c_str());

    if ( ImGui::IsItemActivated() == true &&
         ImGui::IsItemToggledOpen() == false )
    {
      if ( entity != mSelectedEntity )
        mSelectedComponent = entt::null;

      mSelectedEntity = entity;
    }
    else if ( ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
              ImGui::IsItemHovered() )
      ImGui::OpenPopup("##entityContextMenu");

    if ( ImGui::BeginPopup("##entityContextMenu") )
    {
      if ( ImGui::Selectable(("Copy##" + entityId).c_str()) )
      {
        mClipboard.clear();
        mClipboard["type"] = "entities";

        if ( registry.all_of <SceneNode> (entity) == true )
          SerializeChildNode(registry, mClipboard["payload"], entity);
        else
          mEntityMgr->entitySerialize(registry, mClipboard["payload"], entity);
      }

      ImGui::EndPopup();
    }

    ImGui::PopID(); // entityId

    if ( nodeOpened == true )
    {
      each_component(entity, registry,
      [ this, &registry, entity,
        tagType] ( const ComponentType componentType )
      {
        ImGui::PushID(componentType);

        ImGui::BeginDisabled(componentType == tagType);

        if ( ImGui::SmallButton("-##componentRemove") )
        {
          if ( componentType == mEntityMgr->componentType <SceneNode> () )
            RootifyChildNode(registry, entity);

          mEntityMgr->removeLater(entity, componentType);

          if ( mSelectedEntity == entity &&
               mSelectedComponent == componentType )
            componentDeselect();
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

      ImGui::SameLine();

      ImGui::BeginDisabled( mClipboard["type"] != "component" ||
                            mClipboard["payload"].empty() );

      if ( ImGui::SmallButton("Paste##componentPaste") )
        ;

      ImGui::EndDisabled();

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

  ImGui::EndTable(); // EntitiesTable
}

void
EntityManagerUi::ui_show_nodes_table(
  entt::registry& registry )
{
  using fmt::format;
  using compos::Tag;
  using compos::SceneNode;
  using compos::EntityMetaInfo;

  std::pair <entt::entity, entt::entity> nodeToAttach {entt::null, entt::null};
  std::pair <entt::entity, entt::entity> nodeToDestroy {entt::null, entt::null};

  const auto window = ImGui::GetCurrentWindow();

  if ( ImGui::BeginDragDropTargetCustom(window->WorkRect, window->ID) )
  {
    const auto nodePayload = ImGui::AcceptDragDropPayload("sceneNodePayload");

    if ( nodePayload != nullptr )
    {
      IM_ASSERT(nodePayload->DataSize == sizeof(entt::entity));

      const auto eDragged = *(const entt::entity*) nodePayload->Data;

      nodeToAttach = {entt::null, eDragged};
    }

    const auto prefabPayload = ImGui::AcceptDragDropPayload("prefabPayload");

    if ( prefabPayload != nullptr )
    {
      IM_ASSERT(prefabPayload->DataSize == sizeof(Json::Value));

      const auto prefab = *(const Json::Value*) prefabPayload->Data;

      prefabDeserialize(registry, prefab);
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

    if ( mRegistryFilter.query(registry, eParent) == false )
    {
      for ( const auto& childRef : cNode->children )
        each_node( mEntityMgr->get_if_valid(childRef.id, registry) );

      ImGui::PopID(); // nodeId
      return;
    }

    if ( ImGui::SmallButton("-##nodeDestroy") )
      nodeToDestroy = {cNode->parent.get_if_valid(registry), eParent};

    auto flags =  ImGuiTreeNodeFlags_OpenOnArrow |
                  ImGuiTreeNodeFlags_OpenOnDoubleClick |
                  ImGuiTreeNodeFlags_AllowItemOverlap;

    if ( cNode->children.empty() == true )
    {
      flags |= ImGuiTreeNodeFlags_Leaf;
      flags |= ImGuiTreeNodeFlags_Bullet;
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
        componentDeselect();

      mSelectedEntity = eParent;
    }
    else if ( ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
              ImGui::IsItemHovered() )
      ImGui::OpenPopup("##nodeContextMenu");

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

    if ( ImGui::BeginPopup("##nodeContextMenu") )
    {
      if ( ImGui::Selectable(format("Copy##{}", nodeId.str()).c_str()) )
      {
        mClipboard.clear();
        mClipboard["type"] = "entities";
        SerializeChildNode(registry, mClipboard["payload"], eParent);
      }

      ImGui::EndPopup();
    }

    if ( nodeOpened == false )
      return ImGui::PopID(); // nodeId

    for ( const auto& childRef : cNode->children )
      each_node( mEntityMgr->get_if_valid(childRef.id, registry) );

    if ( cNode->children.empty() == false )
      ImGui::TreePop(); // cTag.id

    ImGui::PopID(); // nodeId
  };

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "SceneNodesTable", 1, tableFlags) )
  {
    ImGui::TableNextColumn();

    for ( auto&& [eNode, cNode, cTag] : registry.view <SceneNode, Tag> ().each() )
      if ( cNode.parent.id == null_id )
        each_node(eNode);

    ImGui::EndTable(); // SceneNodesTable
  }

  if ( nodeToAttach.second != entt::null )
    AttachChildNode(registry, nodeToAttach.first, nodeToAttach.second);

  if ( nodeToDestroy.second != entt::null )
  {
    if ( nodeToDestroy.second == mSelectedEntity )
      entityDeselect();

    DestroyChildNode(registry, nodeToDestroy.first, nodeToDestroy.second);
  }
}

void
EntityManagerUi::prefabDeserialize(
  entt::registry& registry,
  const Json::Value& prefab )
{
  using compos::EntityMetaInfo;

  std::unordered_map <EntityId, EntityId,
                      identifier_hash> idMap {};

  for ( const auto& entityId : prefab.getMemberNames() )
  {
    idMap[entityId] = mEntityMgr->idGenerate(entityId);
    mEntityMgr->idRegister(idMap[entityId], entt::null);
  }

  for ( const auto& entityId : prefab.getMemberNames() )
  {
    const auto entity
      = mEntityMgr->entityDeserialize(registry, entityId,
                                      prefab[entityId], idMap);

    auto& cMetaInfo = registry.emplace_or_replace <EntityMetaInfo> (entity);
    cMetaInfo.packageId = mRegistryFilter.package();
  }
}

void
EntityManagerUi::setClipboard(
  const std::string& type,
  const Json::Value& payload )
{
  mClipboard[type] = payload;
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

void
EntityManagerUi::entityDeselect()
{
  mSelectedEntity = entt::null;
  mSelectedComponent = entt::null;
}

void
EntityManagerUi::componentDeselect()
{
  mSelectedComponent = entt::null;
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
