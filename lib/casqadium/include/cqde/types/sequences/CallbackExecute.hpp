#pragma once

#include <cqde/alias.hpp>
#include <cqde/types/sequences/SequenceStep.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>

#include <glm/glm.hpp>


namespace cqde::types
{

class CallbackExecute : public SequenceStep
{
protected:
  std::vector <CallbackId> mCallbacks {};

public:
  CallbackExecute() = default;
  ~CallbackExecute() override = default;

  virtual std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
