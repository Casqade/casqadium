#pragma once

#include <json/forwards.h>

#include <iosfwd>
#include <filesystem>


namespace cqde
{

std::fstream fileOpen(  const std::filesystem::path&,
                        const std::ios::openmode flags );

Json::Value fileParse( const std::filesystem::path& path );

} // namespace cqde
