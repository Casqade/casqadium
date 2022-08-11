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
  const std::string& newQuery )
{
  ImGui::AlignTextToFramePadding();
  ImGui::Text("Find:");

  if ( newQuery.empty() == false )
    mQuery = newQuery;

  ImGui::SameLine();
  mCaseSensitivity = mCaseSensitivityButton.isDown();

  ImGui::SameLine();

  return ImGui::InputTextWithHint(("##" + mHint).c_str(),
                                  mHint.c_str(), &mQuery);
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

  return value.find(query) != std::string::npos;
}

std::string
StringFilter::query() const
{
  return mQuery;
}

} // namespace cqde::ui
