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
  using path = std::filesystem::path;
  using InputBinding = cqde::types::InputBinding;
  using InputBindingComparator = cqde::types::InputBindingComparator;
  using InputBindings = std::set <std::shared_ptr <InputBinding>,
                                  InputBindingComparator>;

  std::map <InputHwCode, InputHwId> mHwControlMap {};

  std::multimap < std::shared_ptr <InputBinding>, InputAxisId,
                  InputBindingComparator> mBindings {};

public:
  InputManager();

  void load( const path& inputConfigPath );
  void save( const path& inputConfigPath ) const;

  void assignBinding( const InputAxisId,
                      const std::shared_ptr <InputBinding> );

  void assignBindings(  const InputAxisId,
                        const InputBindings& );

  void handleAxisInput( const InputHwCode,
                        const float amount,
                        const float direction,
                        entt::registry& );
};

} // namespace cqde::types
