#pragma once

#include <cqde/types/TimeProgress.hpp>
#include <cqde/types/sequences/SequenceStep.hpp>

#include <entt/fwd.hpp>


namespace cqde::types
{

class Delay : virtual public SequenceStep
{
protected:
  TimeProgress mTime {};

public:
  Delay() = default;
  ~Delay() override = default;

  std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
