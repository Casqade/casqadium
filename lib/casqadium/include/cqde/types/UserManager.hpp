#pragma once

#include <cqde/alias.hpp>

#include <vector>
#include <filesystem>


namespace cqde::types
{

class UserManager
{
  using path = std::filesystem::path;

  path mUsersRoot {"users"};

  UserId mUser {"default"};

public:
  UserManager() = default;

  void setUser( const UserId& );
  void setUsersRootPath( const path& );

  UserId user() const;

  path usersRoot() const;
  path userRoot( const UserId& = "" ) const;

  path snapshotsRoot( const UserId& = "" ) const;
  path inputConfigPath( const UserId& = "" ) const;
};

} // namespace cqde::types
