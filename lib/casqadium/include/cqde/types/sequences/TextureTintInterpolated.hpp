#pragma once

#include <cqde/types/SplineCurve.hpp>
#include <cqde/types/sequences/Delay.hpp>

#include <glm/vec4.hpp>


namespace cqde::types
{

class TextureTintInterpolated : public Delay
{
  std::pair <glm::vec4, glm::vec4> mTint {1.0f, 1.0f};

  SplineCurve mSpline {{}, {1.0f, 1.0f}};

  bool mInitFromTextureTint {};

public:
  TextureTintInterpolated() = default;
  ~TextureTintInterpolated() override = default;

  virtual std::string name() const override;

  void init( entt::registry&, const entt::entity ) override;

  bool execute( entt::registry&, const entt::entity ) override;

  void ui_show( const entt::registry&, const entt::entity ) override;

  Json::Value toJson() const override;

  void fromJson( const Json::Value& ) override;
};

} // namespace cqde::types
