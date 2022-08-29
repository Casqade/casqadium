#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <any>
#include <map>
#include <functional>


namespace cqde::ui
{
class CallbackManagerUi;
}

namespace cqde::types
{

class CallbackManager
{
  friend class ui::CallbackManagerUi;

public:
  using CallbackFunc = std::function <void( entt::registry&,
                                            const std::vector <std::any>&)>;

private:
  std::map <CallbackId, CallbackFunc> mCallbacks {};

public:
  CallbackManager() = default;

  void execute( const CallbackId&, entt::registry&,
                const std::vector <std::any>& args ) const;

  void Register( const CallbackId&,
                 const CallbackFunc& );

  std::vector <CallbackId> callbacks() const;
};

} // namespace cqde::types
