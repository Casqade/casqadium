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

  enum class Phase
  {
    Logic,
    Render,

  } phase {};

  bool active {};


  System() = default;
};

} // namespace cqde::types
