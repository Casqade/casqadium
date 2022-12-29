#pragma once

#include <cqde/types/SplineCurve.hpp>
#include <cqde/types/TimeProgress.hpp>
#include <cqde/types/sequences/SequenceStep.hpp>

#include <glm/vec4.hpp>


namespace cqde::types
{

class TextureTintInterpolated : public SequenceStep
{
  SequenceInitializationStatus mInitStatus {};

  bool mInitFromTextureTint {};

  TimeProgress mTime {};

  std::pair <glm::vec4, glm::vec4> mTint {1.0f, 1.0f};

  SplineCurve mSpline {{}, {1.0f, 1.0f}};


public:
  TextureTintInterpolated() = default;
  ~TextureTintInterpolated() override = default;

  std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
