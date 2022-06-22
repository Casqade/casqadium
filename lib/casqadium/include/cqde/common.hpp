#pragma once

#include <entt/fwd.hpp>
#include <entt/core/hashed_string.hpp>

#include <fmt/format.h>


namespace cqde
{

std::fstream fileOpen( const std::string&, const std::ios::openmode flags );

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
