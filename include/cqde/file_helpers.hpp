#pragma once

#include <json/forwards.h>

#include <iosfwd>
#include <filesystem>


namespace cqde
{

bool fileExists( const std::filesystem::path& );

std::fstream fileOpen(  const std::filesystem::path&,
                        const std::ios::openmode flags );

Json::Value fileParse( const std::filesystem::path& path );

} // namespace cqde
