#include <cqde/components/assets/MouseCursorAssetList.hpp>
#include <cqde/types/assets/MouseCursorAssetManager.hpp>

#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>


namespace cqde::compos
{

void
MouseCursorAssetList::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using fmt::format;
  using types::MouseCursorAssetManager;

  if ( ImGui::CollapsingHeader("Mouse cursors", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  const auto cursorList = registry.ctx().get <MouseCursorAssetManager> ().assetIdList();

  static ui::StringFilter cursorFilter {"Mouse Cursor ID"};

  if ( ImGui::SmallButton("+##cursorAdd") )
    ImGui::OpenPopup("##cursorAddPopup");

  if ( ImGui::BeginPopup("##cursorAddPopup") )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    cursorFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    bool cursorFound {};

    for ( const auto& cursorId : cursorList )
    {
      if ( cursorFilter.query(cursorId.str()) == false )
        continue;

      if ( std::find(cursors.begin(), cursors.end(),
                     cursorId) != cursors.end() )
        continue;

      cursorFound = true;

      if ( ImGui::Selectable(cursorId.str().c_str(), false) )
      {
        cursors.insert(cursorId);
        ImGui::CloseCurrentPopup();
        break;
      }
    }

    if ( cursorFound == false )
      ImGui::Text("No mouse cursors matching filter");

    ImGui::EndPopup(); // cursorAddPopup
  }

  ImGui::Separator();

  const auto tableFlags = ImGuiTableFlags_ScrollX |
                          ImGuiTableFlags_ScrollY;

  if ( ImGui::BeginTable( "CursorList", 1, tableFlags) == false )
    return;

  ImGui::TableNextColumn();

  for ( auto iter = cursors.begin();
        iter != cursors.end();
        ++iter )
  {
    ImGui::PushID(std::distance(cursors.begin(), iter));

    if ( ImGui::SmallButton("-##cursorDel") )
      iter = cursors.erase(iter);

    if ( iter == cursors.end() )
    {
      ImGui::PopID();
      break;
    }

    const auto flags =  ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_AllowItemOverlap;

    ImGui::SameLine();
    ImGui::Selectable(iter->str().c_str(), false, flags);

    ImGui::PopID();
  }

  ImGui::EndTable(); // CursorList
}

} // namespace cqde::compos
