#pragma once

#include <cqde/types/SplineCurve.hpp>
#include <cqde/types/TimeProgress.hpp>
#include <cqde/types/sequences/SequenceStep.hpp>


namespace cqde::types
{

class CameraFovInterpolated : public SequenceStep
{
  SequenceInitializationStatus mInitStatus {};

  bool mInitFromCurrentFov {};

  TimeProgress mTime {};

  std::pair <float, float> mFov {1.0f, 1.0f};

  SplineCurve mSpline {{}, {1.0f, 1.0f}};

public:
  CameraFovInterpolated() = default;
  ~CameraFovInterpolated() override = default;

  std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
