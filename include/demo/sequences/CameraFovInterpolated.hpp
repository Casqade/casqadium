#pragma once

#include <cqde/types/SplineCurve.hpp>
#include <cqde/types/sequences/Delay.hpp>


namespace demo
{

class CameraFovInterpolated : public cqde::types::Delay
{
  using SplineCurve = cqde::types::SplineCurve;

  std::pair <float, float> mFov {1.0f, 1.0f};

  SplineCurve mSpline {{}, {1.0f, 1.0f}};

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
