#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/sequences/Delay.hpp>
#include <cqde/types/sequences/CallbackExecute.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class CallbackExecutePeriodic : public Delay, public CallbackExecute
{
public:
  CallbackExecutePeriodic() = default;
  ~CallbackExecutePeriodic() override = default;

  std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
