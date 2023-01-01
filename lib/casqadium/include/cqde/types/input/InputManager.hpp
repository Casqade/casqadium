#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputEvent.hpp>

#include <entt/fwd.hpp>

#include <filesystem>
#include <deque>
#include <map>


namespace cqde::ui
{
class InputManagerUi;
}

namespace cqde::types
{

class InputManager
{
  friend class ui::InputManagerUi;

  using path = std::filesystem::path;

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

  void deserialize( const Json::Value& );
  Json::Value serialize() const;

  void clear();

  static void Validate( const Json::Value& );

  void assignBinding( const InputAxisId&,
                      const std::shared_ptr <InputBinding> );

  void assignBindings(  const InputAxisId&,
                        const InputBindings& );

  void handleAxisInput( const InputHwCode,
                        const float amount,
                        const int8_t direction,
                        entt::registry& );

  bool axisAssigned( const InputAxisId& ) const;
  bool bindingAssigned( const InputAxisId&,
                        const InputHwId ) const;

  std::set <InputAxisId> axes() const;

  std::vector <InputAxisId> axes( const InputHwId ) const;
  std::vector <InputHwId> bindings( const InputAxisId& ) const;

  void setInputHistoryLength( const size_t );
  size_t inputHistoryLength() const;

  const std::deque <InputEvent>& inputHistory() const;
};

} // namespace cqde::types