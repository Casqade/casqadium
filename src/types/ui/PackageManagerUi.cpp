#include <cqde/types/ui/PackageManagerUi.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/EntityManager.hpp>

#include <cqde/types/UndoRedoQueue-inl.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>
#include <cqde/types/assets/TextStringAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <cqde/common.hpp>
#include <cqde/logger.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <json/value.h>
#include <json/writer.h>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <fstream>


namespace cqde::ui
{

bool PackageManagerUi::ConfigState::operator == (
  const ConfigState& other ) const
{
  return
      std::make_tuple(root, packages) ==
      std::make_tuple(other.root, other.packages);
}

PackageManagerUi::PackageManagerUi(
  types::PackageManager* packageMgr )
  : mPackageMgr{packageMgr}
{}

void
PackageManagerUi::ui_show(
  entt::registry& registry )
{
  using fmt::format;
  using types::Package;
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  CQDE_ASSERT_DEBUG(mPackageMgr != nullptr, return);

  if ( ImGui::Begin("Packages", NULL,
                    ImGuiWindowFlags_MenuBar) == false )
    return ImGui::End(); // Packages

  if ( mConfigState.root.empty() == true )
  {
    if ( fileExists(mPackageMgr->manifestPath()) == true )
    {
      mConfigState.root = fileParse(mPackageMgr->manifestPath());
      PackageManager::Validate(mConfigState.root);
    }
    else
    {
      const auto emptyManifest = PackageManager::ManifestJsonReference();
      mConfigState.root = jsonClearComments(emptyManifest);
    }

    mDraggedPackageIndex = -1u;
  }

  auto& packages = mConfigState.root["load_order"];
  std::string entryPoint = mConfigState.root["entry_point"].asString();

  ImGui::Text("Entry point");

  if ( ImGui::BeginCombo("##entryPoint", entryPoint.c_str(),
                         ImGuiComboFlags_HeightLargest) )
  {
    for ( const auto& package : packages )
    {
      if ( ImGui::Selectable(package.asCString(),
                             package.asString() == entryPoint ) )
        entryPoint = package.asString();
    }
    mConfigState.root["entry_point"] = entryPoint;

    ImGui::EndCombo();
  }

  ImGui::Separator();

  bool packageNameInvalid = mPackageNewName.empty() == true;

  if ( packageNameInvalid == false )
    for ( auto& val : packages )
      packageNameInvalid |= mPackageNewName == val.asString();

  ImGui::BeginDisabled(packageNameInvalid);

  if ( ImGui::Button("+##packageAdd") )
  {
    packages.append(mPackageNewName);
    mConfigState.packages[mPackageNewName]["title"] = Json::stringValue;
    mConfigState.packages[mPackageNewName]["version"] = Json::stringValue;
    mConfigState.packages[mPackageNewName]["description"] = Json::stringValue;
    mConfigState.packages[mPackageNewName]["dependencies"] = Json::arrayValue;

    const auto manifestName = Package::ContentFileName(ContentType::Manifest);
    const auto manifestPath = mPackageMgr->rootPath() / mPackageNewName / manifestName;

    if ( fileExists(manifestPath) == true )
    {
      Package newPackage {mPackageNewName};

      try
      {
        newPackage.deserialize(fileParse(manifestPath));

        mConfigState.packages[mPackageNewName] = newPackage.serialize();
      }
      catch ( ... )
      {}
    }
  }

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newPackageId", "New package ID",
                           &mPackageNewName,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "PackagesList", 1, tableFlags) == false )
    return ImGui::End(); // Packages

  ImGui::TableNextColumn();

  for ( Json::ArrayIndex index = 0;
        index < packages.size();
        ++index )
  {
    ImGui::PushID(index);

    if ( ImGui::SmallButton("-##packageDel") )
    {
      if ( mEditedPackageId == packages[index].asString() )
        mEditedPackageId.clear();

      packages.removeIndex(index, nullptr);
    }

    if ( index == packages.size() )
    {
      ImGui::PopID();
      break;
    }

    const bool selected = mEditedPackageId == packages[index].asString();

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(format("{}###", packages[index].asString()).c_str(),
                      selected, flags );

    if ( ImGui::IsItemHovered() == true )
      ImGui::SetTooltip("Drag to reorder");

    if ( ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) == true &&
         ImGui::IsMouseDown(ImGuiMouseButton_Left) == true )
    {
      auto mouseClickPos = ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_Left];

      if (  mouseClickPos.x > ImGui::GetItemRectMin().x &&
            mouseClickPos.x < ImGui::GetItemRectMax().x )
      {
        if ( mDraggedPackageIndex != index &&
             mDraggedPackageIndex >= 0 &&
             mDraggedPackageIndex < packages.size() )
          packages[index].swap(packages[mDraggedPackageIndex]);

        mDraggedPackageIndex = index;
      }
    }

    if ( mDraggedPackageIndex != packages.size() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      mDraggedPackageIndex = packages.size();

    if ( ImGui::IsItemActivated() )
    {
      mPackageWindowOpened = true;
      ImGui::SetWindowFocus("PackageEdit");

      mEditedPackageId = packages[index].asString();

      if ( mConfigState.packages.isMember(mEditedPackageId) == false )
      {
        auto configIter = mHistoryBuffer.current();
        if ( mHistoryBuffer.isValid(configIter) == true )
        {
          auto manifestPath = mPackageMgr->rootPath() /
                              mEditedPackageId /
                              Package::ContentFileName(ContentType::Manifest);
          mConfigState.packages[mEditedPackageId] = fileParse(manifestPath);
          Package::Validate(mConfigState.packages[mEditedPackageId]);

          (*configIter).packages[mEditedPackageId] = mConfigState.packages[mEditedPackageId];
        }
      }
    }

    ImGui::PopID();
  }

  ImGui::EndTable(); // PackagesList

  ui_show_menu_bar(registry);

  ImGui::End(); // Packages

  ui_show_package_window(registry);

  mHistoryBuffer.push(mConfigState);
}

void
PackageManagerUi::ui_show_menu_bar(
  entt::registry& registry )
{
  using fmt::format;
  using types::Package;
  using types::PackageManager;
  using types::InputManager;
  using types::EntityManager;
  using ContentType = types::Package::ContentType;

  using types::AudioAssetManager;
  using types::FontAssetManager;
  using types::GeometryAssetManager;
  using types::TextStringAssetManager;
  using types::TextureAssetManager;

  if ( ImGui::BeginMenuBar() == false )
    return;

  if ( ImGui::MenuItem("Save") )
  {
    const auto& packages = mConfigState.root["load_order"];

    for ( auto& packageId : packages )
    {
      if ( mConfigState.packages.isMember(packageId.asString()) == false )
        continue;

      auto package = mPackageMgr->package(packageId.asString());
      const auto packageManifest = mConfigState.packages[packageId.asString()];

      if ( package == nullptr )
        mPackageMgr->create(packageId.asString(), packageManifest);

      else
        package->save(ContentType::Manifest, packageManifest);
    }

    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    auto fileStream = fileOpen(mPackageMgr->manifestPath(), streamFlags);
    fileStream << Json::writeString(jsonWriter(), mConfigState.root);
    fileStream.close();
  }

  if ( ImGui::BeginMenu("Load") )
  {
    if ( ImGui::MenuItem("Load all") )
    {
      mDraggedPackageIndex = -1u;
      mConfigState.root.clear();
      mConfigState.packages.clear();
    }

    if ( ImGui::MenuItem("Load & apply") )
    {
      mDraggedPackageIndex = -1u;

      mPackageMgr->mPackages.clear();

      registry.ctx().get <EntityManager> ().clear();
      registry.ctx().get <InputManager> ().clear();
      registry.clear();

      registry.ctx().get <AudioAssetManager> ().clear();
      registry.ctx().get <FontAssetManager> ().clear();
      registry.ctx().get <GeometryAssetManager> ().clear();
      registry.ctx().get <TextStringAssetManager> ().clear();
      registry.ctx().get <TextureAssetManager> ().clear();

      try
      {
        mPackageMgr->load(registry);
      }
      catch ( const std::exception& e )
      {
        LOG_ERROR("{}", e.what());
      }

      mConfigState.packages.clear();
      mConfigState.root = fileParse(mPackageMgr->manifestPath());

      PackageManager::Validate(mConfigState.root);
    }

    ImGui::EndMenu();
  }

  ImGui::BeginDisabled(mHistoryBuffer.undoAvailable() == false);

  if ( ImGui::MenuItem("Undo") )
    mConfigState = mHistoryBuffer.undo();

  ImGui::EndDisabled();

  ImGui::BeginDisabled(mHistoryBuffer.redoAvailable() == false);

  if ( ImGui::MenuItem("Redo") )
    mConfigState = mHistoryBuffer.redo();

  ImGui::EndDisabled();

  ImGui::EndMenuBar();
}

void
PackageManagerUi::ui_show_package_window(
  entt::registry& registry )
{
  using types::Package;

  if ( mEditedPackageId.empty() == true )
    return;

  if ( mPackageWindowOpened == false )
    mEditedPackageId.clear();

  if ( ImGui::Begin("PackageEdit", &mPackageWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == false )
    return ImGui::End(); // PackageEdit

  Package {mEditedPackageId}.ui_show(mConfigState.packages[mEditedPackageId]);

  if ( ImGui::CollapsingHeader("Dependencies", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::set <std::string> enabledDeps {};
    auto& dependencies = mConfigState.packages[mEditedPackageId]["dependencies"];

    for ( const auto& dependency : dependencies )
      enabledDeps.insert(dependency.asString());

    for ( const auto& package : mConfigState.root["load_order"] )
    {
      if ( package.asString() == mEditedPackageId )
        continue;

      bool enabled = enabledDeps.count(package.asString());

      ImGui::Checkbox(package.asString().c_str(), &enabled);

      if ( enabled == true )
        enabledDeps.insert(package.asString());
      else
        enabledDeps.erase(package.asString());
    }

    dependencies.clear();

    for ( const auto& dependency : enabledDeps )
      dependencies.append(dependency);
  }

  ImGui::End(); // PackageEdit
}

} // namespace cqde::ui
