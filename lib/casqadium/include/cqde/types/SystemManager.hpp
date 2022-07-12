#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <set>
#include <map>
#include <functional>


namespace cqde::types
{

class SystemManager
{
public:
  using SystemFunc = std::function <void(entt::registry&)>;

private:
  std::map <SystemId, SystemFunc> mSystemsPool {};
  std::map <SystemId, SystemFunc> mSystemsEnabled {};
  std::map <SystemId, SystemFunc> mRenderSystemsEnabled {};

public:
  SystemManager() = default;

  void Register( const SystemId&,
                 const SystemFunc& );

  void enable( const SystemId&,
               const bool rendering );
  void disable( const SystemId& );

  void execute( entt::registry&,
                const bool rendering ) const;

  std::set <SystemId> systems() const;
  std::set <SystemId> systemsEnabled( const bool rendering ) const;


};

} // namespace cqde::types
