#pragma once

#include <string>
#include <vector>


namespace cqde::types
{

struct LaunchParams
{
  std::vector <std::string> args {"CasqadiumApp"};


  LaunchParams() = default;
  LaunchParams( const int argc, char* argv[] );

  std::string executableName() const;

  bool startEditor() const;
};

} // namespace cqde::types
