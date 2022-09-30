#pragma once

#include <cqde/types/sequences/Delay.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/fwd.hpp>

#include <glm/mat4x4.hpp>


namespace cqde::types
{

class TransformInterpolated : public Delay
{
  std::pair <glm::mat4, glm::mat4> mTransform {1.0f, 1.0f};

  glm::vec4 mBezierParams {0.0f, 0.0f, 1.0f, 1.0f};

public:
  TransformInterpolated() = default;

  ~TransformInterpolated() override = default;

  virtual std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry& ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
