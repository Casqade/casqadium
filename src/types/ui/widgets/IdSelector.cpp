#include <cqde/types/ui/widgets/IdSelector.hpp>

#include <imgui.h>


namespace cqde::ui
{

IdSelector::IdSelector(
  const std::string& filterHint,
  const std::string& label )
  : mIdFilter{filterHint}
  , mLabel{label}
{}

bool
IdSelector::select(
  const std::vector <identifier>& idList,
  const UserCallback& callback,
  const UserFilter& filter,
  const ItemComparator& comparator )
{
  bool selected {};
  bool idFound {};

  ItemComparator isItemSelected =
  [] ( const auto& id )
  {
    return false;
  };

  if ( comparator != nullptr )
    isItemSelected = comparator;

  if ( ImGui::IsWindowAppearing() )
    ImGui::SetKeyboardFocusHere(2);

  mIdFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

  for ( const auto& id : idList )
  {
    if ( mIdFilter.query(id.str()) == false )
      continue;

    if (  filter != nullptr &&
          filter(id) == false )
      continue;

    idFound = true;

    if ( ImGui::Selectable(id.str().c_str(), isItemSelected(id)) )
    {
      callback(id);
      selected = true;

      break;
    }
  }

  if ( idFound == false )
    ImGui::Text("No items matching filter");

  return selected;
}

bool
IdSelector::selectCombo(
  identifier& selectedId,
  const std::vector <identifier>& idList,
  const UserFilter& filter )
{
  bool selected {};

  const UserCallback callback =
  [&selectedId] ( const auto& id )
  {
    return selectedId = id;
  };

  return selectCombo(
    selectedId, idList,
    callback, filter );
}

bool
IdSelector::selectCombo(
  const identifier& selectedId,
  const std::vector <identifier>& idList,
  const UserCallback& callback,
  const UserFilter& filter )
{
  bool selected {};

  const ItemComparator comparator =
  [selectedId] ( const auto& id )
  {
    return id == selectedId;
  };

  if ( ImGui::BeginCombo(mLabel.c_str(), selectedId.str().c_str()) )
  {
    selected = select( idList,
      callback, filter, comparator );

    ImGui::EndCombo();
  }

  return selected;
}

bool
IdSelector::selectPopup(
  const std::vector <identifier>& idList,
  const UserCallback& callback,
  const UserFilter& filter )
{
  bool selected {};

  if ( ImGui::BeginPopup(mLabel.c_str()) )
  {
    selected = select( idList,
      callback, filter );

    if ( selected == true )
      ImGui::CloseCurrentPopup();

    ImGui::EndPopup(); // mLabel
  }

  return selected;
}

} // namespace cqde::ui
