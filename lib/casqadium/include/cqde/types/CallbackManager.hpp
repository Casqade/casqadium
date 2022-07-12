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
  std::map <cqde::CallbackId, CallbackFunc> mCallbacks {};

public:
  CallbackManager() = default;

  void Execute( const cqde::CallbackId&, entt::registry&,
                const std::vector <std::any>& args ) const;

  void Register( const cqde::CallbackId&, const CallbackFunc& );


};

} // namespace cqde::types
