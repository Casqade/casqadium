#pragma once

#include <entt/fwd.hpp>

#include <functional>


namespace cqde::types
{

struct System
{
  using Callback = std::function <void(entt::registry&)>;

  Callback callback {};

  enum class Phase
  {
    Update,
    Render,

  } phase {};

  bool active {};


  System() = default;
};

} // namespace cqde::types
