#pragma once

#include <cqde/types/ui/widgets/ToggleButton.hpp>


namespace cqde::ui
{

class StringFilter
{
  ToggleButton mCaseSensitivityButton {"aA"};
  ToggleButton mExcludeQuerriedButton {"!"};

  std::string mHint {};
  std::string mQuery {};

  bool mCaseSensitivity {};
  bool mExcludeQueried {};

public:
  StringFilter( const std::string& hint );

  bool search( const std::string& newQuery = {},
               const int inputTextFlags = 0 );
  bool query( std::string value ) const;

  std::string query() const;

  bool exclusive() const;
};

} // namespace cqde::ui
