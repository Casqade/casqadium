#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/input/InputBinding.hpp>

#include <entt/fwd.hpp>

#include <filesystem>
#include <map>


namespace cqde::types
{

class InputManager
{
  std::map <InputHwCode, InputHwId> mHwControlMap {};

  std::multimap < std::shared_ptr <InputBinding>, InputAxisId,
                  InputBindingComparator> mBindings {};

public:
  InputManager();

  void load( const std::filesystem::path& inputConfigPath );

  void assignBinding( const InputAxisId,
                      const std::shared_ptr <cqde::types::InputBinding> );

  void assignBindings(  const InputAxisId,
                        const std::set <std::shared_ptr <cqde::types::InputBinding>,
                                        cqde::types::InputBindingComparator>& );

  void handleAxisInput( const InputHwCode,
                        const float amount,
                        const float direction,
                        entt::registry& );
};

} // namespace cqde::types
