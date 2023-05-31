#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <functional>


namespace cqde::types
{

struct System
{
  using Callback = std::function <void(entt::registry&)>;

  Callback callback {};
  SystemId id {};

  enum Phase : uint32_t
  {
    Logic = 0x1,
    Render = 0x2,

    Editor = 0x4,

  } phase {};

  bool active {};


  System() = default;
};

} // namespace cqde::types
