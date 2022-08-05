#pragma once

#include <cqde/types/ui/ToggleButton.hpp>


namespace cqde::ui
{

class StringFilter
{
  ToggleButton mCaseSensitivityButton {"aA"};

  std::string mHint {};
  std::string mQuery {};

  bool mCaseSensitivity {};

public:
  StringFilter( const std::string& hint );

  bool search( const std::string& newQuery = {} );
  bool query( std::string value ) const;

  std::string query() const;
};

} // namespace cqde::ui
