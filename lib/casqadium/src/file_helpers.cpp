#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>

#include <fstream>


namespace cqde
{

static std::string
openmodeToString( const std::ios::openmode flags )
{
  std::string result {};

  if ( (flags & std::ios::binary) == std::ios::binary )
    result += "binary,";

  if ( (flags & std::ios::in) == std::ios::in )
    result += "in,";

  if ( (flags & std::ios::out) == std::ios::out )
    result += "out,";

  if ( (flags & std::ios::app) == std::ios::app )
    result += "app,";

  if ( (flags & std::ios::ate) == std::ios::ate )
    result += "ate,";

  if ( (flags & std::ios::trunc) == std::ios::trunc )
    result += "trunc,";

  if ( result.empty() == false )
    result.pop_back();  // get rid of trailing comma

  return result;
}

std::fstream
fileOpen(
  const std::filesystem::path& path,
  const std::ios::openmode flags )
{
  using fmt::format;

  std::filesystem::file_status fileStatus {};
  std::filesystem::path targetPath = path;

  try
  {
    if ( std::filesystem::is_symlink(path) == true )
      targetPath = std::filesystem::read_symlink(path);

    fileStatus = std::filesystem::status(targetPath);
  }
  catch ( const std::filesystem::filesystem_error& e )
  {
    throw std::runtime_error(
      format("Failed to open '{}': {}",
              path.string(), e.code().message()));
  }

  if ( (flags & std::ios::in) == std::ios::in &&
       fileStatus.type() == std::filesystem::file_type::not_found )
    throw std::runtime_error(
      format("Failed to open '{}': {}",
              path.string(), std::strerror(ENOENT)));

  if ( fileStatus.type() != std::filesystem::file_type::not_found &&
       fileStatus.type() != std::filesystem::file_type::regular )
    throw std::runtime_error(
      format("Failed to open '{}': Is not a regular file",
              path.string()));

  std::fstream file {};
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try
  {
    file.open(targetPath, flags);
  }
  catch ( const std::system_error& e )
  {
    throw std::runtime_error(
      format("Failed to open '{}' (mode={}): {}",
              path.string(), openmodeToString(flags),
              std::strerror(errno)));
  }

  return file;
}

Json::Value
fileParse( const std::filesystem::path& path )
{
  using fmt::format;

  std::fstream file = fileOpen( path, std::ios::in );

  try
  {
    return jsonParse(file);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse JSON '{}': {}",
              path.string(), e.what()));
  }
}

} // namespace cqde
