#include <cqde/types/UserManager.hpp>

#include <cqde/types/Package.hpp>

#include <cqde/common.hpp>


namespace cqde::types
{

void
UserManager::setUser(
  const UserId& user )
{
  mUser = user;

  CQDE_ASSERT_DEBUG(mUser.str().empty() == false,
                    mUser = "default");
}

void
UserManager::setUsersRootPath(
  const path& rootPath )
{
  mUsersRoot = rootPath;
}

UserId
UserManager::user() const
{
  return mUser;
}

UserManager::path
UserManager::usersRoot() const
{
  return mUsersRoot;
}

UserManager::path
UserManager::userRoot(
  const UserId& user ) const
{
  if ( user.str().empty() == true )
    return mUsersRoot / mUser.str();

  return mUsersRoot / user.str();
}

UserManager::path
UserManager::snapshotsRoot(
  const UserId& user ) const
{
  if ( user.str().empty() == true )
    return mUsersRoot / mUser.str() / "snapshots";

  return mUsersRoot / user.str() / "snapshots";
}

UserManager::path
UserManager::inputConfigPath(
  const UserId& user ) const
{
  using ContentType = Package::ContentType;

  const auto inputFileName = Package::ContentFileName(ContentType::Input);

  if ( user.str().empty() == true )
    return mUsersRoot / mUser.str() / "config" / inputFileName;

  return mUsersRoot / user.str() / "config" / inputFileName;
}

} // namespace cqde::types
