#pragma once

#include <entt/fwd.hpp>

#include <spdlog/fmt/bundled/format.h>


// assert in debug build
// handle condition safely in release
#define CQDE_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}

namespace cqde
{

template <typename... Args>
std::string format( const fmt::string_view& message, Args&&... args );

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
