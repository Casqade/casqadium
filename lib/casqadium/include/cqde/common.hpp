#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <spdlog/fmt/bundled/format.h>

#include <iosfwd>
#include <bits/fs_fwd.h>


// assert in debug build
// handle condition safely in release
#define LOG_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}

namespace cqde
{

template <typename... Args>
std::string format( const fmt::string_view& message, Args&&... args );

std::string jsonToString( const Json::Value&, const bool withComments = false );

std::fstream fileOpen( const std::filesystem::path&, const std::ios::openmode flags );


Json::CharReaderBuilder   jsonReader();
Json::StreamWriterBuilder jsonWriter();

void jsonValidateArray( const Json::Value&, const Json::Value& reference );
void jsonValidateObject( const Json::Value&, const Json::Value& reference );

Json::Value jsonParse( std::istream& stream );
Json::Value fileParse( const std::filesystem::path& path );


void engineInit( entt::registry& registry );

} // namespace cqde


template <typename... Args>
std::string
cqde::format(
  const fmt::string_view& message,
  Args&&... args )
{
  fmt::memory_buffer buf {};
  fmt::detail::vformat_to ( buf,
                            message,
                            fmt::make_format_args(args...) );

  return std::string(buf.data(), buf.size());
}
