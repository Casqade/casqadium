#include <cqde/types/ui/widgets/StringFilter.hpp>

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
  if ( newQuery.empty() == false )
    mQuery = newQuery;

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
    for ( auto& c : value )
      c = std::tolower(c);

    for ( auto& c : query )
      c = std::tolower(c);
  }

  return value.find(query) != std::string::npos;
}

std::string
StringFilter::query() const
{
  return mQuery;
}

} // namespace cqde::ui
