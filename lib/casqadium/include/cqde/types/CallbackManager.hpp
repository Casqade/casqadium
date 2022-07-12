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


};

} // namespace cqde::types
