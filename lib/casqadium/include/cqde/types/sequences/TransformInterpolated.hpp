#pragma once

#include <cqde/types/SplineCurve.hpp>
#include <cqde/types/sequences/Delay.hpp>


namespace cqde::types
{

class TransformInterpolated : public Delay
{
  std::pair <glm::mat4, glm::mat4> mTransform {1.0f, 1.0f};

  SplineCurve mSpline {{}, {1.0f, 1.0f}};

  bool mUseWorldSpace {};
  bool mInitFromTransform {};

public:
  TransformInterpolated() = default;
  ~TransformInterpolated() override = default;

  virtual std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
