#pragma once

#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace cqde::types
{

class SequenceStep
{
public:
  SequenceStep() = default;

  virtual ~SequenceStep() = default;

  virtual std::string name() const;

  virtual void init( entt::registry&, const entt::entity );

  virtual bool execute( entt::registry&, const entt::entity );

  virtual void ui_show( const entt::registry& );

  virtual Json::Value toJson() const;

  virtual void fromJson( const Json::Value& );
};

} // namespace cqde::types
