#include <cqde/components/SequenceController.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <cqde/types/sequences/SequenceFactory.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::compos
{

void
SequenceController::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry )
{
  using fmt::format;
  using types::SequenceStep;
  using types::SequenceFactory;

  if ( ImGui::CollapsingHeader("Sequence steps", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  auto& sequenceFactory = registry.ctx().get <SequenceFactory> ();

  static ui::StringFilter sequenceFilter {"Sequence ID"};

  static bool stepWindowOpened {};
  static int32_t selectedStep {-1};

  if ( ImGui::SmallButton("+##stepAdd") )
    ImGui::OpenPopup("##stepAddPopup");

  if ( ImGui::BeginPopup("##stepAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    sequenceFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool sequenceFound {};

    for ( const auto& sequence : sequenceFactory.sequences() )
    {
      if ( sequenceFilter.query(sequence) == false )
        continue;

      sequenceFound = true;

      if ( ImGui::Selectable(sequence.c_str(), false) )
      {
        steps.push_back(sequenceFactory.create(sequence));
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( sequenceFound == false )
      ImGui::Text("No sequences matching filter");

    ImGui::EndPopup(); // textureAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "StepsList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = steps.begin();
        iter < steps.end();
        ++iter )
  {
    const auto index = std::distance(steps.begin(), iter);
    ImGui::PushID(index);

    if ( ImGui::SmallButton("-##stepDel") )
    {
      if ( selectedStep >= index )
        selectedStep -= 1;

      iter = steps.erase(iter);
    }

    if ( iter == steps.end() )
    {
      ImGui::PopID(); // index
      break;
    }

    const bool selected = index == selectedStep;

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    if ( ImGui::Selectable( format("{}###", iter->get()->name()).c_str(),
                            selected, flags) )
    {
      selectedStep = index;

      stepWindowOpened = true;
      ImGui::SetWindowFocus("###stepEditWindow");
    }

    static auto iter_dragged = steps.end();

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
             iter_dragged >= steps.begin() &&
             iter_dragged < steps.end() )
          std::swap(*iter, *iter_dragged);

        iter_dragged = iter;
      }
    }

    if ( iter_dragged < steps.end() &&
         ImGui::IsMouseReleased(ImGuiMouseButton_Left) == true )
      iter_dragged = steps.end();

    ImGui::PopID(); // index
  }

  ImGui::EndTable(); // StepsList

  if ( stepWindowOpened == false )
    return;

  if ( selectedStep < 0 ||
       selectedStep >= steps.size() )
    return;

  const auto windowTitle = format("Step '{}'###stepEditWindow",
                                  steps.at(selectedStep)->name());

  if ( ImGui::Begin(windowTitle.c_str(),
                    &stepWindowOpened,
                    ImGuiWindowFlags_MenuBar) )
    steps.at(selectedStep)->ui_show(registry, entity);

  ImGui::End(); // windowTitle
}

} // namespace cqde::compos
