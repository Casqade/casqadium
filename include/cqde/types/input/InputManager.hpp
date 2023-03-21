#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/input/InputEvent.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>

#include <map>
#include <deque>
#include <filesystem>
#include <unordered_map>


namespace cqde::ui
{
class InputManagerUi;
}

namespace cqde::types
{

class InputManager
{
  friend class ui::InputManagerUi;

  using Path = std::filesystem::path;
  using Duration = TimeUtils::Duration;

  using InputBindings = std::set <std::shared_ptr <InputBinding>,
                                  InputBindingComparator>;

  std::map <InputHwCode, InputHwId> mHwControlMap {};

  std::multimap < std::shared_ptr <InputBinding>, InputAxisId,
                  InputBindingComparator> mBindings {};

  std::unordered_map <InputHwCode, Duration> mInputsHeld {};

  std::deque <InputEvent> mInputHistory {};
  size_t mInputHistoryLength {100u};

  Duration mLastPollTimestamp {};

public:
  InputManager();

  void load( const Path& inputConfigPath );
  void save( const Path& inputConfigPath ) const;

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

  void updateInput( entt::registry& );

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
