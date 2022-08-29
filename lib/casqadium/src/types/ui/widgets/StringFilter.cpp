#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <cqde/common.hpp>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace cqde::ui
{

StringFilter::StringFilter(
  const std::string& hint )
  : mHint{hint}
{}

bool
StringFilter::search(
  const std::string& newQuery,
  const int inputTextFlags )
{
  ImGui::PushID(mHint.c_str());

  ImGui::AlignTextToFramePadding();
  ImGui::Text("Find:");

  if ( newQuery.empty() == false )
    mQuery = newQuery;

  ImGui::SameLine();
  mCaseSensitivity = mCaseSensitivityButton.isDown();

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Case sensitivity");

  ImGui::SameLine();
  mExcludeQueried = mExcludeQuerriedButton.isDown();

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("Exclusive filter");

  ImGui::SameLine();

  const bool queryWasChanged =
    ImGui::InputTextWithHint(("##" + mHint).c_str(),
                              mHint.c_str(), &mQuery,
                              inputTextFlags);

  if ( ImGui::IsItemHovered() )
    ImGui::SetTooltip("%s", mHint.c_str());

  ImGui::PopID(); // mHint

  return queryWasChanged;
}

bool
StringFilter::query(
  std::string value ) const
{
  auto query = mQuery;

  if ( mCaseSensitivity == false )
  {
    value = toLowerCase(value);
    query = toLowerCase(query);
  }

  const bool found = value.find(query) != std::string::npos;

  return found ^ mExcludeQueried | query.empty();
}

std::string
StringFilter::query() const
{
  return mQuery;
}

} // namespace cqde::ui
