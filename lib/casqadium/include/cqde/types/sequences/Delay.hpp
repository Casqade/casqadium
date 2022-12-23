#pragma once

#include <cqde/types/sequences/SequenceStep.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class Delay : public SequenceStep
{
  using Duration = TimeUtils::Duration;

protected:

  struct
  {
    Duration elapsed {};
    Duration total {};

  } mTime {};

public:
  Delay() = default;
  ~Delay() override = default;

  virtual std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  double progress() const;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
