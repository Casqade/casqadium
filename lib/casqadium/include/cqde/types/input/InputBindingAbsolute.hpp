#include <cqde/types/input/InputBinding.hpp>


namespace cqde::types
{

struct InputBindingAbsolute : public InputBinding
{
  float curve {};
  float deadzone {};


  InputBindingAbsolute( const cqde::InputHwId&,
                        const Json::Value& );
  InputBindingAbsolute( const cqde::InputHwId&,
                        const float _sensitivity = 1.0f );

  void handleInput( cqde::types::ControlAxis&,
                    const float amount ) const override;

  void ui_show( Json::Value& ) override;

  Json::Value toJson() const override;
};

} // namespace cqde::types
