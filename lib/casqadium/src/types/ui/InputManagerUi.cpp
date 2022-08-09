#include <cqde/types/ui/InputManagerUi.hpp>
#include <cqde/types/input/InputManager.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/UndoRedoQueue-inl.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::types
{
template class
UndoRedoQueue <std::map <PackageId, Json::Value>>;
}

namespace cqde::ui
{

InputManagerUi::InputManagerUi(
  types::InputManager* inputMgr )
  : mInputMgr{inputMgr}
{}

void
InputManagerUi::configApply(
  entt::registry& registry )
{
  using types::PackageManager;

  mInputConfigs.clear();
  mInputMgr->mBindings.clear();

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

  for ( const auto& packageId : pkgMgr.packages() )
    configLoad(packageId, registry);
}

void
InputManagerUi::configLoad(
  const PackageId& packageId,
  entt::registry& registry )
{
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  if ( packageId.str().empty() == true )
    return;

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

  auto package = pkgMgr.package(packageId);

  if ( package != nullptr )
  {
    const auto inputConfigPath = package->contentPath(ContentType::Input);

    const Json::Value packageInputConfig = fileParse(inputConfigPath);

    mInputMgr->deserialize(packageInputConfig);
  }
}

void
InputManagerUi::configSave(
  const PackageId& packageId,
  entt::registry& registry )
{
  using types::PackageManager;
  using ContentType = types::Package::ContentType;

  if ( packageId.str().empty() == true )
    return;

  LOG_TRACE("Writing package '{}' input config", packageId.str());

  auto& pkgMgr = registry.ctx().at <PackageManager> ();

  auto package = pkgMgr.package(packageId);

  if ( package == nullptr )
  {
    LOG_ERROR("Failed to write package '{}' input config: "
              "No such package in PackageManager",
              packageId.str());
    return;
  }

  package->save(ContentType::Input, mInputConfigs[packageId]);
}

void
InputManagerUi::ui_show(
  entt::registry& registry )
{
  using fmt::format;

  using types::PackageManager;
  using types::Package;

  using types::InputBinding;
  using types::InputBindingRelative;

  using ContentType = Package::ContentType;

  CQDE_ASSERT_DEBUG(mInputMgr != nullptr, return);

  if ( ImGui::Begin("Input", NULL, ImGuiWindowFlags_MenuBar) == false )
    return ImGui::End(); // Input

  ImGui::AlignTextToFramePadding();
  ImGui::Text("Package:");

  ImGui::SameLine();
  mPackageFilter.select(registry);

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Input config merged from all packages");

  const auto selectedPackage = mPackageFilter.package();

  ImGui::Separator();

  if ( mInputConfigs.count(selectedPackage) == 0 )
  {
    if ( selectedPackage.str().empty() == false )
    {
      auto& pkgMgr = registry.ctx().at <PackageManager> ();

      auto package = pkgMgr.package(selectedPackage);

      if ( package == nullptr )
        return ImGui::End(); // Input

      const auto inputConfigPath = package->contentPath(ContentType::Input);

      mInputConfigs[selectedPackage] = fileParse(inputConfigPath);
    }
    else
      mInputConfigs[selectedPackage] = mInputMgr->serialize();

    auto configIter = mHistoryBuffer.current();
    if ( mHistoryBuffer.isValid(configIter) == true )
      (*configIter)[selectedPackage] = mInputConfigs[selectedPackage];
  }

  auto& inputConfig = mInputConfigs.at(selectedPackage);

  ImGui::BeginDisabled(mClipboardAxis.empty() == true);

  if ( ImGui::Button("Paste##axisPaste") )
    inputConfig[mClipboardAxis["id"].asString()] = mClipboardAxis["value"];

  ImGui::EndDisabled();

  ImGui::SameLine();
  const bool newAxisInserted = ImGui::Button("+##axisAdd");

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newAxisId", "New axis ID", &mNewAxisName,
                           ImGuiInputTextFlags_AutoSelectAll);

  if ( newAxisInserted == true &&
       mNewAxisName.empty() == false &&
       inputConfig.isMember(mNewAxisName) == false )
    inputConfig[mNewAxisName] = Json::objectValue;

  ImGui::Separator();

  if ( ImGui::BeginTable( "InputConfig", 1, ImGuiTableFlags_ScrollY,
                          {0.0f, ImGui::GetContentRegionAvail().y}) )
  {
    ImGui::TableNextColumn();

    for ( const auto& axisId : inputConfig.getMemberNames() )
    {
      if ( inputConfig.isMember(axisId) == false )
        continue; // handle axes removed during loop

      const bool axisNodeOpened
        = ImGui::TreeNodeEx(axisId.c_str(),
                            ImGuiTreeNodeFlags_OpenOnArrow);

      ImGui::PushID(axisId.c_str());

      if ( ImGui::IsItemHovered() )
      {
        if ( ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) )
        {
          mRenamedAxisId = axisId;
          ImGui::OpenPopup("##axisRenamePopup");
        }
        else if ( ImGui::IsMouseClicked(ImGuiMouseButton_Right) )
          ImGui::OpenPopup("##axisContextMenu");
      }

      ImGui::SameLine();
      if ( ImGui::SmallButton("-##axisRemove") )
      {
        if ( axisId == mSelectedAxis.str() )
        {
          mSelectedAxis = null_id;
          mSelectedBinding = null_id;
        }

        inputConfig.removeMember(axisId);

        if ( axisNodeOpened == true )
          ImGui::TreePop();

        ImGui::PopID();
        continue;
      }

      if ( ImGui::BeginPopup("##axisRenamePopup") )
      {
        ImGui::Text("Rename axis:");
        ImGui::InputText("##axisRename", &mRenamedAxisId,
                         ImGuiInputTextFlags_AutoSelectAll);

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
          inputConfig[mRenamedAxisId].swap(inputConfig[axisId]);

          bool iterationSkipNeeded {};

          if ( inputConfig[axisId].empty() == true )
          {
            inputConfig.removeMember(axisId);
            iterationSkipNeeded = true;
          }

          if ( mSelectedAxis == axisId )
          {
            mSelectedAxis = mRenamedAxisId;
            iterationSkipNeeded = true;
          }

          ImGui::CloseCurrentPopup();

          if ( iterationSkipNeeded == true )
          {
            ImGui::EndPopup();

            if ( axisNodeOpened == true )
              ImGui::TreePop();

            ImGui::PopID();
            continue;
          }
        }
        ImGui::EndPopup();
      }

      if ( ImGui::BeginPopup("##axisContextMenu") )
      {
        if ( ImGui::Selectable(format("Copy '{}'", axisId).c_str()) )
        {
          mClipboardAxis["id"] = axisId;
          mClipboardAxis["value"] = inputConfig[axisId];
        }

        ImGui::BeginDisabled(mClipboardAxis.empty() == true);

        if ( ImGui::Selectable("Paste") )
          inputConfig[axisId] = mClipboardAxis["value"];

        ImGui::EndDisabled();

        ImGui::EndPopup();
      }

      if ( axisNodeOpened == true )
      {
        for ( const auto& bindingId : inputConfig[axisId].getMemberNames() )
        {
          if ( inputConfig[axisId].isMember(bindingId) == false )
            continue; // handle bindings removed during loop

          auto flags = ImGuiTreeNodeFlags_Bullet |
                       ImGuiTreeNodeFlags_NoTreePushOnOpen;

          if ( mSelectedAxis == axisId &&
               mSelectedBinding == bindingId )
            flags |= ImGuiTreeNodeFlags_Selected;

          ImGui::TreeNodeEx(bindingId.c_str(), flags);

          ImGui::PushID(bindingId.c_str());

          if ( ImGui::IsItemActivated() )
          {
            mSelectedAxis = axisId;
            mSelectedBinding = bindingId;
            mBindingWindowOpened = true;
          }

          ImGui::SameLine();
          if ( ImGui::SmallButton("-##bindingRemove") )
          {
            if ( bindingId == mSelectedBinding.str() )
              mSelectedBinding = null_id;

            inputConfig[axisId].removeMember(bindingId);
          }
          ImGui::PopID();
        }

        if ( ImGui::SmallButton("+##bindingAdd") )
          inputConfig[axisId]["Undefined"] = InputBindingRelative{"Undefined"}.toJson();

        ImGui::BeginDisabled(mClipboardBinding.empty() == true);

        ImGui::SameLine();
        if ( ImGui::SmallButton("Paste##bindingPaste") )
          inputConfig[axisId][mClipboardBinding["id"].asString()] = mClipboardBinding["value"];

        ImGui::EndDisabled();

        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    ImGui::EndTable(); // InputConfig
  }

  ui_show_menu_bar(registry);

  ImGui::End(); // Input

  ui_show_binding_window();

  mHistoryBuffer.push(mInputConfigs);
}

void
InputManagerUi::ui_show_menu_bar(
  entt::registry& registry )
{
  using fmt::format;

  if ( ImGui::BeginMenuBar() == false )
    return;

  const auto selectedPackage = mPackageFilter.package();

  if ( ImGui::BeginMenu("Save") )
  {
    if ( selectedPackage.str().empty() == true )
    {
      if ( ImGui::MenuItem("Save merged config") )
        try
        {
          mInputMgr->save("input.json");
        }
        catch ( const std::exception& e )
        {
          LOG_ERROR("Failed to write merged input config to '{}': {}",
                    "input.json", e.what());
        }

      if ( ImGui::IsItemHovered() )
        ImGui::SetTooltip("Writes merged input config to './input.json'");
    }
    else if ( ImGui::MenuItem(format("Save '{}'", selectedPackage.str()).c_str()) )
      configSave(selectedPackage, registry);

    if ( ImGui::MenuItem("Save all") )
      for ( const auto& [packageId, config] : mInputConfigs )
        configSave(packageId, registry);

    ImGui::EndMenu(); // Save
  }

  if ( ImGui::BeginMenu("Load") )
  {
    if ( selectedPackage.str().empty() == false &&
         ImGui::MenuItem(format("Load '{}'", selectedPackage.str()).c_str()) )
      mInputConfigs.erase(selectedPackage);

    if ( ImGui::MenuItem("Load all") )
      mInputConfigs.clear();

    if ( ImGui::MenuItem("Load & apply") )
      configApply(registry);

    ImGui::EndMenu(); // Load
  }

  ImGui::BeginDisabled(mHistoryBuffer.undoAvailable() == false);

  if ( ImGui::MenuItem("Undo") )
    mInputConfigs = mHistoryBuffer.undo();

  ImGui::EndDisabled();

  ImGui::BeginDisabled(mHistoryBuffer.redoAvailable() == false);

  if ( ImGui::MenuItem("Redo") )
    mInputConfigs = mHistoryBuffer.redo();

  ImGui::EndDisabled();

  ImGui::EndMenuBar();
}

void
InputManagerUi::ui_show_binding_window()
{
  using types::InputBinding;

  const auto selectedPackage = mPackageFilter.package();

  if ( mInputConfigs.count(selectedPackage) == 0 )
    return;

  auto& inputConfig = mInputConfigs.at(selectedPackage);

  if ( mSelectedAxis == null_id ||
       mSelectedBinding == null_id )
    return;

  if ( inputConfig.isMember(mSelectedAxis.str()) == false ||
       inputConfig[mSelectedAxis.str()].isMember(mSelectedBinding.str()) == false )
    return;

  if ( mBindingWindowOpened == false )
  {
    mSelectedBinding = null_id;
    return;
  }

  if ( ImGui::Begin("InputBinding", &mBindingWindowOpened) == false )
    return ImGui::End(); // InputBinding

  if ( ImGui::Button("Copy##bindingCopy") )
  {
    mClipboardBinding["id"] = mSelectedBinding.str();
    mClipboardBinding["value"] = inputConfig[mSelectedAxis.str()][mSelectedBinding.str()];
  }

  ImGui::BeginDisabled(mClipboardBinding.empty() == true);

  ImGui::SameLine();
  if ( ImGui::Button("Paste##bindingPaste") )
    inputConfig[mSelectedAxis.str()][mSelectedBinding.str()] = mClipboardBinding["value"];

  ImGui::EndDisabled();

  ImGui::Separator();

  if ( ImGui::CollapsingHeader("Binding ID", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if ( ImGui::BeginCombo("##bindingEditId", mSelectedBinding.str().c_str(),
                           ImGuiComboFlags_HeightLargest) )
    {
      for ( const auto& [inputHwCode, inputHwId] : mInputMgr->mHwControlMap )
      {
        using olc::Key;
        using olc::MouseInputId;

        auto bindingPrev = mSelectedBinding;

        if ( inputHwCode > Key::NONE &&
             inputHwCode < InputHwCode(MouseInputId::ENUM_END) )
        {
          const std::string inputHwIdPositive = "+" + inputHwId.str();
          const std::string inputHwIdNegative = "-" + inputHwId.str();

          bool selected = (mSelectedBinding == inputHwIdPositive);

          if ( ImGui::Selectable(inputHwIdPositive.c_str(), selected) )
            mSelectedBinding = inputHwIdPositive;

          selected = (mSelectedBinding == inputHwIdNegative);

          if ( ImGui::Selectable(inputHwIdNegative.c_str(), selected) )
            mSelectedBinding = inputHwIdNegative;
        }
        else if ( inputHwCode == InputHwCode(MouseInputId::ENUM_END) )
        {
          bool selected = (mSelectedBinding == inputHwId);

          if ( ImGui::Selectable(inputHwId.str().c_str(), selected) )
            mSelectedBinding = inputHwId;
        }

        if ( mSelectedBinding != bindingPrev )
        {
          auto& selectedBinding = inputConfig[mSelectedAxis.str()][mSelectedBinding.str()];
          auto& selectedBindingPrev = inputConfig[mSelectedAxis.str()][bindingPrev.str()];

          selectedBinding.swap(selectedBindingPrev);

          if ( selectedBindingPrev.empty() == true )
            inputConfig[mSelectedAxis.str()].removeMember(bindingPrev.str());
        }
      }
      ImGui::EndCombo();
    }
  }

  Json::Value& bindingConfig = inputConfig[mSelectedAxis.str()][mSelectedBinding.str()];

  InputBinding::FromJson({}, bindingConfig)->ui_show(bindingConfig);

  ImGui::End(); // InputBinding
}

} // namespace cqde::ui
