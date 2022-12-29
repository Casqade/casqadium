#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <any>
#include <map>
#include <functional>


namespace cqde::types
{

class CallbackManager
{
public:
  using CallbackArgs = std::vector <std::any>;
  using CallbackFunc = std::function <void( entt::registry&,
                                            const CallbackArgs& )>;

private:
  std::unordered_map <CallbackId, CallbackFunc,
                      identifier_hash> mCallbacks {};

  std::vector <std::pair <CallbackFunc, CallbackArgs>> mDelayedCallbacks {};

public:
  CallbackManager() = default;

  void execute( const CallbackId&, entt::registry&,
                const CallbackArgs& args ) const;

  void executeLater( const CallbackFunc&, const CallbackArgs& = {} );

  void delayedExecution( entt::registry& );

  void Register( const CallbackId&,
                 const CallbackFunc& );

  std::vector <CallbackId> callbacks() const;
  std::vector <CallbackId> callbacksSorted() const;
};

} // namespace cqde::types
