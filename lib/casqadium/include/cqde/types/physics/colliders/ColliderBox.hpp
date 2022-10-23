#pragma once

#include <cqde/types/physics/colliders/Collider.hpp>


namespace cqde::types
{

class ColliderBox : public Collider
{
  rp3d::BoxShape* mShape {};

public:
  ColliderBox();
  ~ColliderBox();

  void shapeEnable() override;
  void shapeDisable() override;

  void ui_show( const entt::registry& ) override;

  Json::Value serialize() const override;

  void shapeStateApply() override;
  void shapeStateValidate() override;

  rp3d::CollisionShape* shape() const override;

  std::string type() const override;
};

} // namespace cqde::types
