#pragma once

#include <cqde/types/sequences/Delay.hpp>


namespace demo
{

class CameraFovInterpolated : public cqde::types::Delay
{
  std::pair <float, float> mFov {1.0f, 1.0f};

  glm::vec4 mBezierParams {0.0f, 0.0f, 1.0f, 1.0f};

  bool mInitFromCurrentFov {};

public:
  CameraFovInterpolated() = default;
  ~CameraFovInterpolated() override = default;

  virtual std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace demo
