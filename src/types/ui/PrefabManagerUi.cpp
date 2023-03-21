#include <cqde/types/ui/PrefabManagerUi.hpp>
#include <cqde/types/ui/EntityManagerUi.hpp>

#include <cqde/types/EntityManager.hpp>
#include <cqde/types/PackageManager.hpp>
#include <cqde/types/PrefabManager.hpp>

#include <cqde/types/UndoRedoQueue-inl.hpp>

#include <cqde/components/SceneNode.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/logger.hpp>


#include <json/value.h>
#include <json/writer.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <fstream>


namespace cqde::ui
{

PrefabManagerUi::PrefabManagerUi(
  types::PrefabManager* prefabMgr )
  : mPrefabMgr{prefabMgr}
{}

void
PrefabManagerUi::prefabsApply(
  const entt::registry& registry )
{
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  mPrefabsState.clear();
  mPrefabMgr->clear();

  try
  {
    const auto& pkgMgr = registry.ctx().get <PackageManager> ();

    for ( const auto& packageId : pkgMgr.packages() )
    {
      const auto package = pkgMgr.package(packageId);
      CQDE_ASSERT_DEBUG(package != nullptr, continue);

      const auto prefabDbPath = package->contentPath(ContentType::Prefabs);

      if ( fileExists(prefabDbPath) == false )
        continue;

      mPrefabMgr->deserialize( fileParse(prefabDbPath) );
    }
  }
  catch ( const std::exception& e )
  {
    LOG_ERROR("{}", e.what());
  }
}

void
PrefabManagerUi::prefabsSave(
  const PackageId& packageId,
  entt::registry& registry )
{
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  CQDE_ASSERT_DEBUG(packageId.str().empty() == false, return);

  LOG_TRACE("Writing package '{}' input config", packageId.str());

  const auto& pkgMgr = registry.ctx().get <PackageManager> ();

  const auto package = pkgMgr.package(packageId);

  if ( package == nullptr )
  {
    LOG_ERROR("Failed to write package '{}' prefab database: "
              "No such package in PackageManager",
              packageId.str());
    return;
  }

  package->save(ContentType::Prefabs, mPrefabsState[packageId]);
}

void
PrefabManagerUi::ui_show(
  entt::registry& registry )
{
  using fmt::format;
  using types::Package;
  using types::PackageManager;
  using types::EntityManager;
  using compos::EntityMetaInfo;
  using ContentType = types::Package::ContentType;

  CQDE_ASSERT_DEBUG(mPrefabMgr != nullptr, return);

  if ( ImGui::Begin("Prefabs", NULL,
                    ImGuiWindowFlags_MenuBar) == false )
    return ImGui::End(); // Prefabs

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Package:");

    ImGui::SameLine();
    mPackageFilter.select(registry);

    if ( mPackageFilter.package().str().empty() == true &&
         ImGui::IsItemHovered() )
      ImGui::SetTooltip("Live prefabs state");

    mPrefabFilter.search();
  }

  const auto selectedPackage = mPackageFilter.package();

  ImGui::Separator();

  if ( mPrefabsState.count(selectedPackage) == 0 )
  {
    if ( selectedPackage.str().empty() == false )
    {
      auto& pkgMgr = registry.ctx().get <PackageManager> ();

      auto package = pkgMgr.package(selectedPackage);

      if ( package == nullptr )
        return ImGui::End(); // Prefabs

      const auto prefabsPath = package->contentPath(ContentType::Prefabs);

      if ( fileExists(prefabsPath) == true )
        mPrefabsState[selectedPackage] = fileParse(prefabsPath);

      else
        mPrefabsState[selectedPackage] = Json::objectValue;

    }
    else
      mPrefabsState[selectedPackage] = mPrefabMgr->serialize();

    auto configIter = mHistoryBuffer.current();
    if ( mHistoryBuffer.isValid(configIter) == true )
      (*configIter)[selectedPackage] = mPrefabsState[selectedPackage];
  }

  auto& prefabsState = mPrefabsState.at(selectedPackage);

  const bool disabled = mNewPrefabId.empty() == true ||
                        prefabsState.isMember(mNewPrefabId) == true;

  ImGui::BeginDisabled(disabled);

  if ( ImGui::Button("+##prefabAdd") )
    prefabsState[mNewPrefabId] = Json::objectValue;

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newPrefabId", "New prefab ID", &mNewPrefabId,
                           ImGuiInputTextFlags_AutoSelectAll);

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "PrefabsList", 1, tableFlags) == false )
    return ImGui::End(); // Prefabs

  ImGui::TableNextColumn();

  for ( const auto& prefabId : prefabsState.getMemberNames() )
  {
    if ( prefabsState.isMember(prefabId) == false )
      continue; // handle prefabs removed during loop

    if ( mPrefabFilter.query(prefabId) == false )
      continue;

    ImGui::PushID(prefabId.c_str());

    if ( ImGui::SmallButton("-##prefabRemove") )
    {
      if ( prefabId == mSelectedPrefabId.str() )
        mSelectedPrefabId = null_id;

      prefabsState.removeMember(prefabId);

      ImGui::PopID(); // prefabId
      continue;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    const bool selected = prefabId == mSelectedPrefabId.str();

    ImGui::SameLine();

    if ( ImGui::Selectable(prefabId.c_str(), selected, flags) )
      mSelectedPrefabId = prefabId;

    if ( ImGui::IsItemHovered() )
    {
      if ( ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) )
      {
        mRenamedPrefabId = prefabId;
        ImGui::OpenPopup("##prefabRenamePopup");
      }
      else if ( ImGui::IsMouseReleased(ImGuiMouseButton_Right) )
        ImGui::OpenPopup("##prefabContextMenu");

      ImGui::SetTooltip("%s", format("{} entities",
                                     prefabsState[prefabId].size()).c_str());
    }

    if ( ImGui::BeginDragDropSource() )
    {
      ImGui::SetDragDropPayload("prefabPayload",
                                &prefabsState[prefabId],
                                sizeof(prefabsState[prefabId]));

      ImGui::Text("%s", format("Prefab '{}': {} entities", prefabId,
                                prefabsState[prefabId].size()).c_str());

      ImGui::EndDragDropSource();
    }

    if ( ImGui::BeginDragDropTarget() )
    {
      const auto entitiesPayload = ImGui::AcceptDragDropPayload("entitiesPayload");

      if ( entitiesPayload != nullptr )
      {
        IM_ASSERT(entitiesPayload->DataSize == sizeof(std::vector <entt::entity>));

        const auto entities = *(const std::vector <entt::entity>*) entitiesPayload->Data;

        const auto& entityManager = registry.ctx().get <EntityManager> ();

        for ( const auto entity : entities )
          entityManager.entitySerialize(registry, prefabsState[prefabId], entity,
                                        {entityManager.componentType <EntityMetaInfo> ()});
      }

      const auto nodePayload = ImGui::AcceptDragDropPayload("sceneNodePayload");

      if ( nodePayload != nullptr )
      {
        IM_ASSERT(nodePayload->DataSize == sizeof(entt::entity));

        const auto& entityManager = registry.ctx().get <EntityManager> ();

        const auto eDragged = *(const entt::entity*) nodePayload->Data;
        SerializeChildNode(registry, prefabsState[prefabId], eDragged,
                           {entityManager.componentType <EntityMetaInfo> ()});
      }

      ImGui::EndDragDropTarget();
    }

    if ( ImGui::BeginPopup("##prefabRenamePopup") )
    {
      ImGui::SetKeyboardFocusHere();

      ImGui::Text("Rename prefab:");

      const bool prefabRenamed
        = ImGui::InputText("##prefabRename", &mRenamedPrefabId,
                           ImGuiInputTextFlags_AutoSelectAll |
                           ImGuiInputTextFlags_EnterReturnsTrue);

      const bool prefabExists = mRenamedPrefabId != prefabId &&
                                prefabsState.isMember(mRenamedPrefabId);

      if ( prefabExists == true )
        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
                           "%s", format("Prefab '{}' already exists",
                                        mRenamedPrefabId).c_str());

      if ( prefabRenamed == true )
      {
        if ( prefabExists == false )
          ImGui::CloseCurrentPopup();

        if ( prefabsState.isMember(mRenamedPrefabId) == false )
        {
          prefabsState[mRenamedPrefabId] = prefabsState[prefabId];
          prefabsState.removeMember(prefabId);

          if ( mSelectedPrefabId == prefabId )
            mSelectedPrefabId = mRenamedPrefabId;

          ImGui::EndPopup(); // prefabRenamePopup

          ImGui::PopID(); // prefabId
          continue;
        }
      }

      ImGui::EndPopup(); // prefabRenamePopup
    }

    if ( ImGui::BeginPopup("##prefabContextMenu") )
    {
      if ( ImGui::Selectable(format("Copy '{}'", prefabId).c_str()) )
      {
        auto& entityManagerUi = registry.ctx().get <EntityManagerUi> ();
        entityManagerUi.setClipboard("entities", prefabsState[prefabId]);
      }

      ImGui::EndPopup(); // prefabContextMenu
    }

    ImGui::PopID(); // prefabId
  }

  ImGui::EndTable(); // PrefabsList

  ui_show_menu_bar(registry);

  ImGui::End(); // Prefabs

  mHistoryBuffer.push(mPrefabsState);
}

void
PrefabManagerUi::ui_show_menu_bar(
  entt::registry& registry )
{
  using fmt::format;
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  if ( ImGui::BeginMenuBar() == false )
    return;

  const auto selectedPackage = mPackageFilter.package();

  if ( ImGui::BeginMenu("Save") )
  {
    if (  selectedPackage.str().empty() == false &&
          ImGui::MenuItem(format("Save '{}'", selectedPackage.str()).c_str()) )
      prefabsSave(selectedPackage, registry);

    if ( ImGui::MenuItem("Save all") )
      for ( const auto& [packageId, config] : mPrefabsState )
        if ( packageId.str().empty() == false )
          prefabsSave(packageId, registry);

    if ( ImGui::MenuItem("Save all & apply") )
    {
      for ( const auto& [packageId, config] : mPrefabsState )
        if ( packageId.str().empty() == false )
          prefabsSave(packageId, registry);

      prefabsApply(registry);
    }

    if ( selectedPackage.str().empty() == true &&
         ImGui::MenuItem("Apply live state") )
    {
      mPrefabMgr->clear();
      mPrefabMgr->deserialize(mPrefabsState[selectedPackage]);
    }

    ImGui::EndMenu(); // Save
  }

  if ( ImGui::BeginMenu("Load") )
  {
    if ( selectedPackage.str().empty() == false &&
         ImGui::MenuItem(format("Load '{}'", selectedPackage.str()).c_str()) )
      mPrefabsState.erase(selectedPackage);

    if ( ImGui::MenuItem("Load all") )
      mPrefabsState.clear();

    if ( ImGui::MenuItem("Load & apply") )
      prefabsApply(registry);

    ImGui::EndMenu();
  }

  ImGui::BeginDisabled(mHistoryBuffer.undoAvailable() == false);

  if ( ImGui::MenuItem("Undo") )
    mPrefabsState = mHistoryBuffer.undo();

  ImGui::EndDisabled();

  ImGui::BeginDisabled(mHistoryBuffer.redoAvailable() == false);

  if ( ImGui::MenuItem("Redo") )
    mPrefabsState = mHistoryBuffer.redo();

  ImGui::EndDisabled();

  ImGui::EndMenuBar();
}

} // namespace cqde::ui
