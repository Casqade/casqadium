#pragma once

#include <cqde/types/enums.hpp>
#include <cqde/types/TimeProgress.hpp>
#include <cqde/types/sequences/SequenceStep.hpp>

#include <glm/vec3.hpp>


namespace cqde::types
{

class TransformManipulate : public SequenceStep
{
  MathFunctionType mFunction {MathFunctionType::Constant};
  TransformType mOperation {TransformType::Translate};

  bool mUseWorldSpace {};
  bool mTransformRelative {};

  glm::vec3 mAxis {};

  TimeProgress mTime {};

  double mAmplitude {1.0};
  double mPeriod {1.0};
  double mPhase {};
  double mOffset {};


protected:
  virtual double value( const double input ) const;

public:
  TransformManipulate() = default;
  ~TransformManipulate() override = default;

  std::string name() const override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
