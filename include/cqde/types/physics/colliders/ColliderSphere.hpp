#pragma once

#include <cqde/types/physics/colliders/Collider.hpp>


namespace cqde::types
{

class ColliderSphere : public Collider
{
protected:
  rp3d::SphereShape* mShape {};

  void shapeInit( entt::registry& ) override;
  void shapeDestroy() override;

  Json::Value shapeSerialize() const override;

  void shapeDeserialize( entt::registry&, const Json::Value& ) override;

public:
  ~ColliderSphere();

  void ui_show( const entt::registry& ) override;

  rp3d::CollisionShape* shape() const override;

  std::string type() const override;
};

} // namespace cqde::types
