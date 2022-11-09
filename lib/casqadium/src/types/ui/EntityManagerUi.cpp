#include <cqde/types/ui/EntityManagerUi.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/SystemManager.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/components/Tag.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/EntityMetaInfo.hpp>
#include <cqde/components/CasqadiumEditorInternal.hpp>

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
  : mSelectedComponent{entt::null, entt::null}
  , mEntityMgr{entityMgr}
{}

void
EntityManagerUi::ui_show(
  entt::registry& registry )
{
  CQDE_ASSERT_DEBUG(mEntityMgr != nullptr, return);

  if ( ImGui::Begin("Registry view", NULL,
                    ImGuiWindowFlags_MenuBar) == false )
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

  ui_show_menu_bar(registry);

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

  const auto [selectedEntity, selectedComponent] = mSelectedComponent;

  if ( selectedEntity == entt::null ||
       selectedComponent == entt::null ||
       registry.valid(selectedEntity) == false )
  {
    ImGui::Text("No component selected");

    ImGui::End(); // Component view
    return;
  }

  const auto entityId = registry.get <Tag> (selectedEntity).id;

  auto component = entt::resolve(mSelectedComponent.second);

  if ( !component )
  {
    ImGui::Text("Selected component is invalid");

    ImGui::End(); // Component view
    return;
  }

  const auto componentName = component_name(mSelectedComponent.second);

  ImGui::BulletText("%s", format("{} -> {}", entityId.str(), componentName).c_str());

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::Spacing();

  if ( component_is_empty(mSelectedComponent.second) == true )
  {
    ImGui::Text("No data for this component");
    ImGui::End(); // Component view
    return;
  }

  auto componentGet = component.func("get"_hs);
  auto componentInstance = componentGet.invoke({}, entt::forward_as_meta(registry), selectedEntity);

  if ( componentInstance )
  {
    auto componentUiShow = component.func("ui_edit_props"_hs);
    componentUiShow.invoke(componentInstance, selectedEntity, entt::forward_as_meta(registry));
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
    mEntityMgr->prefabDeserialize(registry, mClipboard["payload"],
                                  mRegistryFilter.package());

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
  using compos::CasqadiumEditorInternal;

  const auto window = ImGui::GetCurrentWindow();

  if ( ImGui::BeginDragDropTargetCustom(window->WorkRect, window->ID) )
  {
    const auto payload = ImGui::AcceptDragDropPayload("prefabPayload");

    if ( payload != nullptr )
    {
      IM_ASSERT(payload->DataSize == sizeof(Json::Value));

      const auto prefab = *(const Json::Value*) payload->Data;

      mEntityMgr->prefabDeserialize(registry, prefab,
                                    mRegistryFilter.package());
    }
    ImGui::EndDragDropTarget();
  }

  std::set <ComponentType> protectedTypes
  {
    mEntityMgr->componentType <Tag> (),
    mEntityMgr->componentType <CasqadiumEditorInternal> ()
  };

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable("EntitiesTable", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  registry.sort <Tag> (
  [] ( const Tag& lhs, const Tag& rhs )
  {
    return lhs.id.str() < rhs.id.str();
  });

  registry.view <Tag> ().each(
  [ this, &registry,
    protectedTypes] ( const entt::entity entity,
                      const Tag& cTag )
  {
    if ( mRegistryFilter.query(registry, entity) == false )
      return;

    const auto entityId = cTag.id.str();

    ImGui::PushID(entityId.c_str());

    if ( ImGui::SmallButton("-##entityRemove") )
    {
      mEntityMgr->removeLater(entity);
      entityDeselect(entity);
    }

    ImGui::PopID(); // entityId

    auto nodeFlags =  ImGuiTreeNodeFlags_OpenOnArrow |
                      ImGuiTreeNodeFlags_AllowItemOverlap |
                      ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if ( entitySelected(entity) == true )
      nodeFlags |= ImGuiTreeNodeFlags_Selected;

    ImGui::SameLine();

    const bool nodeOpened = ImGui::TreeNodeEx(entityId.c_str(), nodeFlags);

    ImGui::PushID(entityId.c_str());

    if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
         ImGui::IsItemToggledOpen() == false &&
         ImGui::IsItemHovered() )
    {
      if ( entitiesMultipleSelection() == false )
      {
        entitiesDeselect();
        entitySelect(entity);
      }
      else if ( entitySelected(entity) == true )
        entityDeselect(entity);
      else
        entitySelect(entity);
    }
    else if ( ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
              ImGui::IsItemHovered() )
      ImGui::OpenPopup("##entityContextMenu");

    if ( ImGui::BeginDragDropSource() )
    {
      ImGui::SetDragDropPayload("entitiesPayload",
                                &mSelectedEntities,
                                sizeof(mSelectedEntities));

      ImGui::Text("%s", format("{} entities",
                               mSelectedEntities.size()).c_str());

      ImGui::EndDragDropSource();
    }

    if ( ImGui::BeginDragDropTarget() )
    {
      const auto componentPayload = ImGui::AcceptDragDropPayload("componentPayload");

      if ( componentPayload != nullptr )
      {
        IM_ASSERT(componentPayload->DataSize == sizeof(mSelectedComponent));

        const auto cDragged = *(const decltype(mSelectedComponent)*) componentPayload->Data;

        const auto cDraggedName = component_name(cDragged.second);

        Json::Value componentBuffer = Json::objectValue;

        mEntityMgr->componentSerialize(registry, componentBuffer,
                                       cDragged.first, cDragged.second);

        mEntityMgr->componentRemove(cDragged.second, entity, registry);

        mEntityMgr->componentDeserialize(registry, entity, cDraggedName,
                                         componentBuffer[cDraggedName]);
      }

      ImGui::EndDragDropTarget();
    }

    if ( ImGui::BeginPopup("##entityContextMenu") )
    {
      if ( ImGui::Selectable(("Copy##" + entityId).c_str()) )
      {
        mClipboard.clear();
        mClipboard["type"] = "entities";

        for ( const auto selectedEntity : mSelectedEntities )
          if ( registry.all_of <SceneNode> (selectedEntity) == true )
            SerializeChildNode( registry, mClipboard["payload"], selectedEntity,
                                {mEntityMgr->componentType <EntityMetaInfo> ()});
          else
            mEntityMgr->entitySerialize(registry, mClipboard["payload"], selectedEntity,
                                        {mEntityMgr->componentType <EntityMetaInfo> ()});
      }

      ImGui::EndPopup(); // entityContextMenu
    }

    ImGui::PopID(); // entityId

    if ( nodeOpened == true )
    {
      each_component(entity, registry,
      [ this, &registry, entity, &entityId,
        protectedTypes] ( const ComponentType componentType )
      {
        const auto entityComponentPair = std::make_pair(entity, componentType);

        ImGui::PushID(componentType);

        ImGui::BeginDisabled(protectedTypes.count(componentType) > 0);

        if ( ImGui::SmallButton("-##componentRemove") )
        {
          if ( componentType == mEntityMgr->componentType <SceneNode> () )
            RootifyChildNode(registry, entity);

          mEntityMgr->removeLater(entity, componentType);

          if ( mSelectedComponent == entityComponentPair )
            componentDeselect();
        }

        ImGui::EndDisabled();

        auto flags = ImGuiTreeNodeFlags_Leaf |
                     ImGuiTreeNodeFlags_AllowItemOverlap |
                     ImGuiTreeNodeFlags_NoTreePushOnOpen;

        if ( mSelectedComponent == entityComponentPair )
          flags |= ImGuiTreeNodeFlags_Selected;

        ImGui::SameLine();

        const auto componentName = component_name(componentType);

        const bool selected = mSelectedComponent == entityComponentPair;

        if ( ImGui::Selectable(componentName.c_str(), selected) )
          mSelectedComponent = {entity, componentType};

        if (  ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
              ImGui::IsItemHovered() )
          ImGui::OpenPopup("##componentContextMenu");

        if (  componentType != mEntityMgr->componentType <Tag> () &&
              componentType != mEntityMgr->componentType <SceneNode> () &&
              ImGui::BeginDragDropSource() )
        {
          const auto cDragged = std::make_pair(entity, componentType);
          ImGui::SetDragDropPayload("componentPayload",
                                    &cDragged,
                                    sizeof(cDragged));

          ImGui::Text("%s", format("Component '{}'", componentName).c_str());

          ImGui::EndDragDropSource();
        }

        if ( ImGui::BeginDragDropTarget() )
        {
          const auto componentPayload = ImGui::AcceptDragDropPayload("componentPayload");

          if ( componentPayload != nullptr )
          {
            IM_ASSERT(componentPayload->DataSize == sizeof(mSelectedComponent));

            const auto cDragged = *(const decltype(mSelectedComponent)*) componentPayload->Data;

            const auto cDraggedName = component_name(cDragged.second);

            Json::Value componentBuffer = Json::objectValue;

            mEntityMgr->componentSerialize(registry, componentBuffer,
                                           cDragged.first, cDragged.second);

            mEntityMgr->componentRemove(cDragged.second, entity, registry);

            mEntityMgr->componentDeserialize(registry, entity, cDraggedName,
                                             componentBuffer[cDraggedName]);
          }

          ImGui::EndDragDropTarget();
        }

        if ( ImGui::BeginPopup("##componentContextMenu") )
        {
          ImGui::BeginDisabled(protectedTypes.count(componentType) > 0);

          if ( ImGui::Selectable(("Copy##" + componentName).c_str()) )
          {
            mClipboard.clear();
            mClipboard["type"] = "component";

            mEntityMgr->componentSerialize( registry, mClipboard["payload"],
                                            entity, componentType);
          }

          ImGui::EndDisabled();

          ImGui::EndPopup(); // componentContextMenu
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
      {
        const auto componentName = mClipboard["payload"].begin().key().asString();
        const auto componentType = mEntityMgr->componentType(componentName);

        mEntityMgr->componentRemove(componentType, entity, registry);

        mEntityMgr->componentDeserialize(registry, entity,
                                         componentName,
                                         mClipboard["payload"][componentName]);
      }

      ImGui::EndDisabled();

      ImGui::Spacing();

      if ( ImGui::BeginPopup("##componentAddPopup") )
      {
        if ( ImGui::IsWindowAppearing() )
          ImGui::SetKeyboardFocusHere(2);

        mNewComponentFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

        bool componentsFound {};

        auto componentNames = mEntityMgr->componentNames();
        std::sort(componentNames.begin(), componentNames.end());

        for ( const auto& componentName : componentNames )
        {
          if ( mNewComponentFilter.query(componentName) == false )
            continue;

          const auto componentType = mEntityMgr->componentType(componentName);

          if ( protectedTypes.count(componentType) )
            continue;

          componentsFound = true;

          if ( ImGui::Selectable(componentName.c_str(), false) )
          {
            mEntityMgr->componentAdd(componentType, entity, registry);

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

      mEntityMgr->prefabDeserialize(registry, prefab,
                                    mRegistryFilter.package());
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

    if ( entitySelected(eParent) == true )
      flags |= ImGuiTreeNodeFlags_Selected;

    ImGui::SameLine();
    const bool nodeOpened = ImGui::TreeNodeEx(nodeId.str().c_str(), flags);

    if ( ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
         ImGui::IsItemToggledOpen() == false &&
         ImGui::IsItemHovered() )
    {
      if ( entitiesMultipleSelection() == false )
      {
        entitiesDeselect();
        entitySelect(eParent);
      }
      else if ( entitySelected(eParent) == true )
        entityDeselect(eParent);
      else
        entitySelect(eParent);
    }
    else if ( ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
              ImGui::IsItemHovered() )
      ImGui::OpenPopup("##nodeContextMenu");

    if ( ImGui::BeginDragDropSource() )
    {
      ImGui::SetDragDropPayload("sceneNodePayload",
                                &eParent,
                                sizeof(eParent));
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
    if ( entitySelected(nodeToDestroy.second) == true )
      entityDeselect(nodeToDestroy.second);

    DestroyChildNode(registry, nodeToDestroy.first, nodeToDestroy.second);
  }
}

void
EntityManagerUi::ui_show_menu_bar(
  entt::registry& registry )
{
  using fmt::format;
  using types::PackageManager;
  using types::EntityManager;
  using ContentType = types::Package::ContentType;

  if ( ImGui::BeginMenuBar() == false )
    return;

  const auto& packageManager = registry.ctx().at <PackageManager> ();

  const auto selectedPackage = mRegistryFilter.package();

  if ( ImGui::BeginMenu("Save") )
  {
    if (  selectedPackage.str().empty() == false &&
          ImGui::MenuItem(format("Save '{}'", selectedPackage.str()).c_str()) )
      entitiesSave(selectedPackage, registry);

    if ( ImGui::MenuItem("Save all") )
      for ( const auto& packageId : packageManager.packages() )
        if ( packageId.str().empty() == false )
          entitiesSave(packageId, registry);

    ImGui::EndMenu(); // Save
  }

  if ( ImGui::MenuItem("Reload") )
  {
    entitiesDeselect();
    componentDeselect();

    registry.ctx().at <EntityManager> ().clear();
    registry.clear();

    try
    {
      for ( const auto& packageId : packageManager.packages() )
      {
        auto package = packageManager.package(packageId);

        if ( package == nullptr )
        {
          LOG_ERROR("Failed to load package '{}' entity registry: "
                    "No such package in PackageManager",
                    packageId.str());
          continue;
        }

        mEntityMgr->load(package->contentPath(ContentType::Entities),
                         packageId, registry);
      }
    }
    catch ( const std::exception& e )
    {
      LOG_ERROR("{}", e.what());
    }
  }

  ImGui::EndMenuBar();
}

void
EntityManagerUi::entitiesSave(
  const PackageId& packageId,
  entt::registry& registry ) const
{
  using compos::Tag;
  using compos::EntityMetaInfo;
  using compos::CasqadiumEditorInternal;

  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  const auto& packageManager = registry.ctx().at <PackageManager> ();

  const auto package = packageManager.package(packageId);

  if ( package == nullptr )
  {
    LOG_ERROR("Failed to write package '{}' entity registry: "
              "No such package in PackageManager",
              packageId.str());
    return;
  }

  const auto entitiesJson = mEntityMgr->serialize(packageId, registry,
  {
    mEntityMgr->componentType <Tag> (),
    mEntityMgr->componentType <EntityMetaInfo> (),
    mEntityMgr->componentType <CasqadiumEditorInternal> (),
  });

  package->save(ContentType::Entities, entitiesJson);
}

void
EntityManagerUi::setClipboard(
  const std::string& type,
  const Json::Value& payload )
{
  mClipboard[type] = payload;
}

void
EntityManagerUi::setEntitiesMultipleSelection(
  const bool entitiesMultipleSelection )
{
  mEntitiesMultipleSelectionEnabled = entitiesMultipleSelection;
}

void
EntityManagerUi::entitySelect(
  const entt::entity entity )
{
  if ( entitySelected(entity) == false )
    mSelectedEntities.push_back(entity);
}

void
EntityManagerUi::componentSelect(
  const entt::entity entity,
  const ComponentType component )
{
  mSelectedComponent = {entity, component};
}

void
EntityManagerUi::entitiesDeselect()
{
  mSelectedEntities.clear();
}

void
EntityManagerUi::entityDeselect(
  const entt::entity entity )
{
  for ( auto iter = mSelectedEntities.begin();
        iter != mSelectedEntities.end();
        ++iter )
    if ( *iter == entity )
    {
      mSelectedEntities.erase(iter);
      return;
    }
}

void
EntityManagerUi::componentDeselect()
{
  mSelectedComponent = {entt::null, entt::null};
}

bool
EntityManagerUi::entitiesMultipleSelection() const
{
  return mEntitiesMultipleSelectionEnabled;
}

bool
EntityManagerUi::entitySelected(
  const entt::entity entity ) const
{
  for ( const auto selectedEntity : mSelectedEntities )
    if ( selectedEntity == entity )
      return true;

  return false;
}

std::vector <entt::entity>
EntityManagerUi::selectedEntities() const
{
  return mSelectedEntities;
}

std::pair <entt::entity, ComponentType>
EntityManagerUi::selectedComponent() const
{
  return mSelectedComponent;
}

} // namespace cqde::types
