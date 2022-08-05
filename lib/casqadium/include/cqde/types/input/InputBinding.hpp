#pragma once

#include <cqde/alias.hpp>

#include <json/forwards.h>

#include <set>


namespace cqde::types
{

struct InputBinding
{
  InputHwId inputId;
  float sensitivity;


  InputBinding( const cqde::InputHwId&,
                const Json::Value& );
  InputBinding( const cqde::InputHwId&,
                const float sensitivity = 1.0f );

  virtual void handleInput( cqde::types::ControlAxis&,
                            const float amount ) const;

  virtual void ui_show( Json::Value& );

  virtual Json::Value toJson() const;

  static std::shared_ptr <InputBinding> FromJson( const InputHwId&, const Json::Value& );
};

struct InputBindingComparator
{
  using is_transparent = void;

  bool operator () ( const std::shared_ptr <InputBinding>& lhs, const std::shared_ptr <InputBinding>& rhs ) const;
  bool operator () ( const cqde::InputHwId& lhs, const std::shared_ptr <InputBinding>& rhs ) const;
  bool operator () ( const std::shared_ptr <InputBinding>& lhs, const cqde::InputHwId& rhs ) const;
};

} // namespace cqde::types
