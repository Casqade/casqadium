#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputEvent.hpp>

#include <entt/fwd.hpp>

#include <filesystem>
#include <deque>
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

  size_t mInputHistoryLength {100u};
  std::deque <InputEvent> mInputHistory {};

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

  std::set <InputAxisId> axes() const;
  std::set <InputHwId> bindings( const InputAxisId& ) const;

  void setInputHistoryLength( const size_t );
  size_t inputHistoryLength() const;

  const std::deque <InputEvent>& inputHistory() const;

};

} // namespace cqde::types
