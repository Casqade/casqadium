#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/sequences/RandomDelay.hpp>
#include <cqde/types/sequences/CallbackExecute.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class CallbackExecuteAPeriodic : public RandomDelay, public CallbackExecute
{
public:
  CallbackExecuteAPeriodic() = default;
  ~CallbackExecuteAPeriodic() override = default;

  std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
