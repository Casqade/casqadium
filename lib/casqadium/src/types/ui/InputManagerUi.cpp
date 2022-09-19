#include <cqde/types/ui/InputManagerUi.hpp>

#include <cqde/types/input/InputManager.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <cqde/common.hpp>
#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/types/PackageManager.hpp>
#include <cqde/types/UserManager.hpp>
#include <cqde/types/UndoRedoQueue-inl.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <entt/entity/registry.hpp>

#include <json/writer.h>

#include <imgui.h>
#include <imgui_stdlib.h>


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
  using types::UserManager;
  using ContentType = types::Package::ContentType;

  mInputConfigs.clear();
  mInputMgr->clear();

  const auto& packageManager = registry.ctx().at <PackageManager> ();
  const auto& userManager = registry.ctx().at <UserManager> ();

  for ( const auto& packageId : packageManager.packages() )
  {
    const auto package = packageManager.package(packageId);
    CQDE_ASSERT_DEBUG(package != nullptr, continue);

    const auto inputConfigPath = package->contentPath(ContentType::Input);

    if ( fileExists(inputConfigPath) == false )
      continue;

    mInputMgr->deserialize( fileParse(inputConfigPath) );
  }

  const auto userInputPath = userManager.inputConfigPath();

  if ( fileExists(userInputPath) == true )
    mInputMgr->deserialize( fileParse(userInputPath) );
}

void
InputManagerUi::configSave(
  const PackageId& packageId,
  entt::registry& registry )
{
  using types::PackageManager;
  using types::UserManager;
  using ContentType = types::Package::ContentType;

  if ( packageId.str().empty() == true )
  {
    LOG_TRACE("Writing user input config");

    const auto& userManager = registry.ctx().at <UserManager> ();

    const auto userInputPath = userManager.inputConfigPath();

    const auto streamFlags = std::ios::out |
                             std::ios::trunc |
                             std::ios::binary;

    try
    {
      auto fileStream = fileOpen(userInputPath, streamFlags);
      fileStream << Json::writeString(jsonWriter(), mInputConfigs[packageId]);
    }
    catch ( const std::exception& e )
    {
      using fmt::format;

      throw std::runtime_error(
        format("Failed to write user input config to '{}': {}",
               userInputPath.string(), e.what()));
    }

    return;
  }

  LOG_TRACE("Writing package '{}' input config", packageId.str());

  const auto& pkgMgr = registry.ctx().at <PackageManager> ();

  const auto package = pkgMgr.package(packageId);

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

  using types::UserManager;
  using types::PackageManager;
  using types::Package;

  using types::InputBinding;
  using types::InputBindingRelative;

  using ContentType = Package::ContentType;

  CQDE_ASSERT_DEBUG(mInputMgr != nullptr, return);

  if ( ImGui::Begin("Input", NULL,
                    ImGuiWindowFlags_MenuBar) == false )
    return ImGui::End(); // Input

  if ( ImGui::CollapsingHeader("Filter", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Package:");

    ImGui::SameLine();
    mPackageFilter.select(registry);

    if ( mPackageFilter.package().str().empty() == true &&
         ImGui::IsItemHovered() )
      ImGui::SetTooltip("User input config");

    mAxisFilter.search();
  }

  ImGui::Separator();

  const auto selectedPackage = mPackageFilter.package();

  if ( mInputConfigs.count(selectedPackage) == 0 )
  {
    if ( selectedPackage.str().empty() == false )
    {
      auto& packageManager = registry.ctx().at <PackageManager> ();

      auto package = packageManager.package(selectedPackage);

      if ( package == nullptr )
        return ImGui::End(); // Input

      const auto inputConfigPath = package->contentPath(ContentType::Input);

      if ( fileExists(inputConfigPath) == true )
        mInputConfigs[selectedPackage] = fileParse(inputConfigPath);

      else
        mInputConfigs[selectedPackage] = Json::objectValue;
    }
    else
    {
      auto& userManager = registry.ctx().at <UserManager> ();

      const auto inputConfigPath = userManager.inputConfigPath();

      if ( fileExists(inputConfigPath) == true )
        mInputConfigs[selectedPackage] = fileParse(inputConfigPath);

      else
        mInputConfigs[selectedPackage] = mInputMgr->serialize();
    }

    auto configIter = mHistoryBuffer.current();
    if ( mHistoryBuffer.isValid(configIter) == true )
      (*configIter)[selectedPackage] = mInputConfigs[selectedPackage];
  }

  auto& inputConfig = mInputConfigs.at(selectedPackage);

  ImGui::BeginDisabled(mClipboardAxis.empty() == true);

  if ( ImGui::Button("Paste##axisPaste") )
    inputConfig[mClipboardAxis["id"].asString()] = mClipboardAxis["value"];

  ImGui::EndDisabled();

  const bool disabled = mNewAxisName.empty() == true ||
                        inputConfig.isMember(mNewAxisName) == true;

  ImGui::BeginDisabled(disabled);

  ImGui::SameLine();
  if ( ImGui::Button("+##axisAdd") )
    inputConfig[mNewAxisName] = Json::objectValue;

  ImGui::EndDisabled();

  ImGui::SameLine();
  ImGui::InputTextWithHint("##newAxisId", "New axis ID", &mNewAxisName,
                           ImGuiInputTextFlags_AutoSelectAll);

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "InputConfig", 1, tableFlags) )
  {
    ImGui::TableNextColumn();

    for ( const auto& axisId : inputConfig.getMemberNames() )
    {
      if ( inputConfig.isMember(axisId) == false )
        continue; // handle axes removed during loop

      if ( mAxisFilter.query(axisId) == false )
        continue;

      ImGui::PushID(axisId.c_str());

      if ( ImGui::SmallButton("-##axisRemove") )
      {
        if ( axisId == mSelectedAxis.str() )
        {
          mSelectedAxis = null_id;
          mSelectedBinding = null_id;
        }

        inputConfig.removeMember(axisId);

        ImGui::PopID(); // axisId
        continue;
      }
      ImGui::PopID(); // axisId

      const auto axisFlags =  ImGuiTreeNodeFlags_OpenOnArrow |
                              ImGuiTreeNodeFlags_AllowItemOverlap;

      ImGui::SameLine();

      const bool axisNodeOpened = ImGui::TreeNodeEx(axisId.c_str(), axisFlags);

      ImGui::PushID(axisId.c_str());

      if ( ImGui::IsItemHovered() )
      {
        if ( ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) )
        {
          mRenamedAxisId = axisId;
          ImGui::OpenPopup("##axisRenamePopup");
        }
        else if ( ImGui::IsMouseReleased(ImGuiMouseButton_Right) )
          ImGui::OpenPopup("##axisContextMenu");
      }

      if ( ImGui::BeginPopup("##axisRenamePopup") )
      {
        ImGui::SetKeyboardFocusHere();

        ImGui::Text("Rename axis:");

        const bool axisRenamed = ImGui::InputText("##axisRename", &mRenamedAxisId,
                                                   ImGuiInputTextFlags_AutoSelectAll |
                                                   ImGuiInputTextFlags_EnterReturnsTrue);

        const bool axisExists = mRenamedAxisId != axisId &&
                                inputConfig.isMember(mRenamedAxisId);

        if ( axisExists == true )
          ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
                             "%s", format("Axis '{}' already exists",
                                          mRenamedAxisId).c_str());

        if ( axisRenamed == true )
        {
          if ( axisExists == false )
            ImGui::CloseCurrentPopup();

          if ( inputConfig.isMember(mRenamedAxisId) == false )
          {
            inputConfig[mRenamedAxisId] = inputConfig[axisId];
            inputConfig.removeMember(axisId);

            if ( mSelectedAxis == axisId )
              mSelectedAxis = mRenamedAxisId;

            ImGui::EndPopup(); // axisRenamePopup

            if ( axisNodeOpened == true )
              ImGui::TreePop(); //axisId

            ImGui::PopID(); // axisId
            continue;
          }
        }

        ImGui::EndPopup(); // axisRenamePopup
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

        ImGui::EndPopup(); // axisContextMenu
      }

      if ( axisNodeOpened == true )
      {
        for ( const auto& bindingId : inputConfig[axisId].getMemberNames() )
        {
          if ( inputConfig[axisId].isMember(bindingId) == false )
            continue; // handle bindings removed during loop

          ImGui::PushID(bindingId.c_str());

          if ( ImGui::SmallButton("-##bindingRemove") )
          {
            if ( bindingId == mSelectedBinding.str() )
              mSelectedBinding = null_id;

            inputConfig[axisId].removeMember(bindingId);
          }

          const bool selected = mSelectedAxis == axisId &&
                                mSelectedBinding == bindingId;

          ImGui::SameLine();
          if ( ImGui::Selectable(bindingId.c_str(), selected) )
          {
            mSelectedAxis = axisId;
            mSelectedBinding = bindingId;

            mBindingWindowOpened = true;
            ImGui::SetWindowFocus("InputBinding");
          }
          ImGui::PopID(); // bindingId
        }

        if ( ImGui::SmallButton("+##bindingAdd") )
          ImGui::OpenPopup("##bindingAddPopup");

        ImGui::BeginDisabled(mClipboardBinding.empty() == true);

        ImGui::SameLine();
        if ( ImGui::SmallButton("Paste##bindingPaste") )
          inputConfig[axisId][mClipboardBinding["id"].asString()] = mClipboardBinding["value"];

        ImGui::EndDisabled();

        if ( ImGui::BeginPopup("##bindingAddPopup") )
        {
          if ( ImGui::IsWindowAppearing() )
            ImGui::SetKeyboardFocusHere(2);

          mBindingFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

          bool bindingsFound {};

          ImGui::Spacing();
          ImGui::Separator();

          const auto configPrev = inputConfig[axisId];

          for ( const auto& [inputHwCode, inputHwId] : mInputMgr->mHwControlMap )
          {
            using olc::Key;
            using olc::MouseInputId;

            if ( inputHwCode > Key::NONE &&
                 inputHwCode < InputHwCode(MouseInputId::ENUM_END) )
            {
              const std::string inputHwIdPositive = "+" + inputHwId.str();
              const std::string inputHwIdNeutral = "~" + inputHwId.str();
              const std::string inputHwIdNegative = "-" + inputHwId.str();

              if ( mBindingFilter.query(inputHwIdPositive) == true &&
                   inputConfig[axisId].isMember(inputHwIdPositive) == false )
              {
                bindingsFound = true;

                if ( ImGui::Selectable(inputHwIdPositive.c_str(), false) )
                  inputConfig[axisId][inputHwIdPositive] = InputBindingRelative{inputHwIdPositive}.toJson();
              }

              if ( mBindingFilter.query(inputHwIdNeutral) == true &&
                   inputConfig[axisId].isMember(inputHwIdNeutral) == false )
              {
                bindingsFound = true;

                if ( ImGui::Selectable(inputHwIdNeutral.c_str(), false) )
                  inputConfig[axisId][inputHwIdNeutral] = InputBindingRelative{inputHwIdNeutral}.toJson();
              }

              if ( mBindingFilter.query(inputHwIdNegative) == true &&
                   inputConfig[axisId].isMember(inputHwIdNegative) == false )
              {
                bindingsFound = true;

                if ( ImGui::Selectable(inputHwIdNegative.c_str(), false) )
                  inputConfig[axisId][inputHwIdNegative] = InputBindingRelative{inputHwIdNegative}.toJson();
              }
            }
            else if ( inputHwCode == InputHwCode(MouseInputId::ENUM_END) )
            {
              if ( mBindingFilter.query(inputHwId.str()) == true &&
                   inputConfig[axisId].isMember(inputHwId.str()) == false )
              {
                bindingsFound = true;

                if ( ImGui::Selectable(inputHwId.str().c_str(), false) )
                  inputConfig[axisId][inputHwId.str()] = InputBindingRelative{inputHwId.str()}.toJson();
              }
            }
          }

          if ( bindingsFound == false )
            ImGui::Text("No binding IDs matching filter");

          if ( inputConfig[axisId] != configPrev )
            ImGui::CloseCurrentPopup();

          ImGui::EndPopup(); // bindingAddPopup
        }

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
    const std::string menuItemLabel
      = selectedPackage.str().empty()
      ? "Save user input config"
      : format("Save '{}'", selectedPackage.str());

    if ( ImGui::MenuItem(menuItemLabel.c_str()) )
      configSave(selectedPackage, registry);

    if ( ImGui::MenuItem("Save all") )
      for ( const auto& [packageId, config] : mInputConfigs )
        configSave(packageId, registry);

    if ( ImGui::MenuItem("Save all & apply") )
    {
      for ( const auto& [packageId, config] : mInputConfigs )
        configSave(packageId, registry);

      configApply(registry);
    }

    if ( selectedPackage.str().empty() == true &&
         ImGui::MenuItem("Apply user input config") )
    {
      mInputMgr->clear();
      mInputMgr->deserialize(mInputConfigs[selectedPackage]);
    }

    ImGui::EndMenu(); // Save
  }

  if ( ImGui::BeginMenu("Load") )
  {
    const std::string menuItemLabel
      = selectedPackage.str().empty()
      ? "Load user input config"
      : format("Load '{}'", selectedPackage.str());

    if ( ImGui::MenuItem(menuItemLabel.c_str()) )
      mInputConfigs.erase(selectedPackage);

    if ( ImGui::MenuItem("Load all") )
      mInputConfigs.clear();

    if ( ImGui::MenuItem("Load all & apply") )
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

  if ( ImGui::Begin("InputBinding", &mBindingWindowOpened,
                    ImGuiWindowFlags_HorizontalScrollbar) == false )
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

    if ( ImGui::BeginCombo("##bindingEditId", mSelectedBinding.str().c_str()) )
    {
      if ( ImGui::IsWindowAppearing() )
        ImGui::SetKeyboardFocusHere(2);

      mBindingFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

      ImGui::Spacing();
      ImGui::Separator();

      bool bindingsFound {};

      for ( const auto& [inputHwCode, inputHwId] : mInputMgr->mHwControlMap )
      {
        using olc::Key;
        using olc::MouseInputId;

        auto bindingPrev = mSelectedBinding;

        if ( inputHwCode > Key::NONE &&
             inputHwCode < InputHwCode(MouseInputId::ENUM_END) )
        {
          const std::string inputHwIdPositive = "+" + inputHwId.str();
          const std::string inputHwIdNeutral = "~" + inputHwId.str();
          const std::string inputHwIdNegative = "-" + inputHwId.str();

          if ( mBindingFilter.query(inputHwIdPositive) == true )
          {
            bindingsFound = true;

            const bool selected = (mSelectedBinding == inputHwIdPositive);

            if ( ImGui::Selectable(inputHwIdPositive.c_str(), selected) )
              mSelectedBinding = inputHwIdPositive;
          }

          if ( mBindingFilter.query(inputHwIdNeutral) == true )
          {
            bindingsFound = true;

            const bool selected = (mSelectedBinding == inputHwIdNeutral);

            if ( ImGui::Selectable(inputHwIdNeutral.c_str(), selected) )
              mSelectedBinding = inputHwIdNeutral;
          }

          if ( mBindingFilter.query(inputHwIdNegative) == true )
          {
            bindingsFound = true;

            const bool selected = (mSelectedBinding == inputHwIdNegative);

            if ( ImGui::Selectable(inputHwIdNegative.c_str(), selected) )
              mSelectedBinding = inputHwIdNegative;
          }
        }
        else if ( inputHwCode == InputHwCode(MouseInputId::ENUM_END) )
        {
          if ( mBindingFilter.query(inputHwId.str()) == true )
          {
            bindingsFound = true;

            const bool selected = (mSelectedBinding == inputHwId);

            if ( ImGui::Selectable(inputHwId.str().c_str(), selected) )
              mSelectedBinding = inputHwId;
          }
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

      if ( bindingsFound == false )
        ImGui::Text("No binding IDs matching filter");

      ImGui::EndCombo();
    }
  }

  Json::Value& bindingConfig = inputConfig[mSelectedAxis.str()][mSelectedBinding.str()];

  InputBinding::FromJson({}, bindingConfig)->ui_show(bindingConfig);

  ImGui::End(); // InputBinding
}

} // namespace cqde::ui
