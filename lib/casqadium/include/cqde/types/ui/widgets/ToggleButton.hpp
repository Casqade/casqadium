#pragma once

#include <string>


namespace cqde::ui
{

class ToggleButton
{
  std::pair <std::string, std::string> mLabels {};
  bool mValue {};

public:
  ToggleButton( const std::string& label );

  ToggleButton( const std::string& labelOn,
                const std::string& labelOff );

  bool isDown();

  void setValue( const bool );
  bool value() const;
};

} // namespace cqde::ui
