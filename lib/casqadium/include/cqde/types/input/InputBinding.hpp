#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <set>


namespace cqde::types
{

struct InputBinding
{
  cqde::InputHwId inputId;
  float sensitivity;


  InputBinding( const cqde::InputHwId&,
                const float sensitivity = 1.0f );

  virtual void handleInput( cqde::types::ControlAxis&,
                            const float amount ) const;
};

struct InputBindingComparator
{
  using is_transparent = void;

  bool operator () ( const std::shared_ptr <InputBinding>& lhs, const std::shared_ptr <InputBinding>& rhs ) const;
  bool operator () ( const cqde::InputHwId& lhs, const std::shared_ptr <InputBinding>& rhs ) const;
  bool operator () ( const std::shared_ptr <InputBinding>& lhs, const cqde::InputHwId& rhs ) const;
};

} // namespace cqde::types
