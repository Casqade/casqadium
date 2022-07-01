#include <cqde/types/input/InputBinding.hpp>


namespace cqde::types
{

struct InputBindingRelative : public InputBinding
{
  InputBindingRelative( const cqde::InputHwId&,
                        const Json::Value& );
  InputBindingRelative( const cqde::InputHwId&,
                        const float sensitivity = 1.0f );

  void handleInput( cqde::types::ControlAxis&,
                    const float amount ) const override;

  Json::Value toJson() const override;
};

} // namespace cqde::types
