#include <ECS/PackageLoader.hpp>

#include <fstream>


class FileOperationError : public std::exception
{
protected:
  std::string mFilename;

public:
  FileOperationError( const std::string& filename );
};

FileOperationError::FileOperationError( const std::string& filename )
  : mFilename(filename)
{}


class FileNotFound : public FileOperationError
{
public:
  FileNotFound( const std::string& filename );
  const char* what() const noexcept override;
};

FileNotFound::FileNotFound( const std::string& filename )
  : FileOperationError(filename)
{}

const char*
FileNotFound::what() const noexcept
{
  return ("File '" + mFilename + "' does not exist").c_str();
}


class FileTypeInvalid : public FileOperationError
{
public:
  FileTypeInvalid( const std::string& filename );
  const char* what() const noexcept override;
};

FileTypeInvalid::FileTypeInvalid( const std::string& filename )
  : FileOperationError(filename)
{}

const char*
FileTypeInvalid::what() const noexcept
{
  return ("File '" + mFilename + "' is not a package file").c_str();
}


class FileCreationFailed : public FileOperationError
{
public:
  FileCreationFailed( const std::string& filename );
  const char* what() const noexcept override;
};

FileCreationFailed::FileCreationFailed( const std::string& filename )
  : FileOperationError(filename)
{}

const char*
FileCreationFailed::what() const noexcept
{
  return ("Failed to create file '" + mFilename + "'").c_str();
}


namespace ECS
{

std::string packageLoadOrderFile = "load_order.json";
std::string packageFilename = "package.json";


namespace Components
{

void
Package::open( const std::filesystem::path& filename )
{
  std::filesystem::file_status package = std::filesystem::status( filename );

  if ( package.type() == std::filesystem::file_type::not_found )
    throw FileNotFound( filename.u8string() );

  if ( package.type() != std::filesystem::file_type::regular )
    throw FileTypeInvalid( filename.u8string() );


}

void
Package::save( const std::filesystem::path& filename )
{
  std::filesystem::file_status package = std::filesystem::status( filename );

  if ( package.type() == std::filesystem::file_type::not_found )
  {
    std::ofstream file( filename.u8string(), std::ios::out );
    if ( file.is_open() == false )
      throw FileCreationFailed( filename.u8string() );

    file.close();

    package = std::filesystem::status( filename );
  }

  if ( package.type() != std::filesystem::file_type::regular )
    throw FileTypeInvalid( filename.u8string() );

// todo: save registry
}

void
Package::load( const entt::registry& globalRegistry )
{

}

} // namespace Components

bool
PackageLoader::Load()
{
  if ( std::filesystem::exists(mPackagesRootPath / packageLoadOrderFile) )
  {
    return true;
  }
  Components::PackageManager packageManager;

  for ( const auto& packageDir : std::filesystem::directory_iterator{mPackagesRootPath} )
  {
      if ( packageDir.is_directory() == false )
        continue;

      if ( std::filesystem::exists( packageDir.path() / packageFilename ) == true )
      {
//        packageManager.packages.insert()
      }
  }

  return true;
}

} // namespace ECS
