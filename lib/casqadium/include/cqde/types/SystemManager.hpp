#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/System.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>

#include <vector>


namespace cqde::ui
{
class SystemManagerUi;
}

namespace cqde::types
{

class SystemManager
{
  friend class ui::SystemManagerUi;

  using Phase = System::Phase;
  using Callback = System::Callback;

  using SystemsPool = std::vector <System>;
  using SystemsIterator = SystemsPool::iterator;

  SystemsPool mSystems {};

  SystemsIterator systemIter( const SystemId& );

public:
  SystemManager() = default;

  void Register( const SystemId&,
                 const Callback&,
                 const Phase );

  void activate( const SystemId& );
  void deactivate( const SystemId& );
  void deactivate();

  void execute( entt::registry&,
                const Phase );

  static void Validate( const Json::Value& );

  void deserialize( const Json::Value& );
  Json::Value serialize() const;

  bool isActive( const SystemId& ) const;

  Phase phase( const SystemId& );

  std::vector <SystemId> systems() const;
  std::vector <SystemId> systems( const Phase ) const;
  std::vector <SystemId> systemsActive( const Phase ) const;
};

} // namespace cqde::types
