#pragma once

#include <entt/fwd.hpp>

#include <json/forwards.h>
#include <fmt/format.h>

#include <filesystem>


namespace cqde
{

Json::CharReaderBuilder   jsonReader();
Json::StreamWriterBuilder jsonWriter();

std::fstream fileOpen( const std::filesystem::path&, const std::ios::openmode flags );

Json::Value jsonParse( std::istream& stream );
Json::Value fileParse( const std::filesystem::path& path );

std::string jsonToString( const Json::Value&, const bool withComments = false );

void engineInit( entt::registry& registry );

template <typename... Args>
std::string
format(
  const fmt::string_view message,
  Args&&... args )
{
  fmt::memory_buffer buf {};
  fmt::detail::vformat_to ( buf,
                            message,
                            fmt::make_format_args(args...) );

  return std::string(buf.data(), buf.size());
}

} // namespace cqde
