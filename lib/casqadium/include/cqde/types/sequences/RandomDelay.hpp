#pragma once

#include <cqde/types/sequences/Delay.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class RandomDelay : public Delay
{
protected:
  using Duration = TimeUtils::Duration;

  struct
  {
    Duration min {};
    Duration max {};

  } mDelayRange {};

  SequenceInitializationStatus mInitStatus {};

public:
  RandomDelay() = default;
  ~RandomDelay() override = default;

  std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
