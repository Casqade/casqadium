#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/System.hpp>

#include <entt/fwd.hpp>

#include <vector>
#include <unordered_map>


namespace cqde::types
{

class SystemManager
{
  using Phase = System::Phase;
  using Callback = System::Callback;

  std::unordered_map <SystemId, System,
                      identifier_hash> mSystems {};

public:
  SystemManager() = default;

  void Register( const SystemId&,
                 const Callback&,
                 const Phase );

  void activate( const SystemId& );
  void deactivate( const SystemId& );

  void execute( entt::registry&,
                const Phase );

  std::vector <SystemId> systems() const;
  std::vector <SystemId> systemsActive( const Phase ) const;
};

} // namespace cqde::types
