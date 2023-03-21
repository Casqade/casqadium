#include <cqde/types/physics/colliders/ColliderSphere.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/collision/shapes/SphereShape.h>


namespace cqde::types
{

const static Json::Value colliderSphereJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& jsonRadius = root["radius"];
  jsonRadius = ValueType::realValue;
  jsonRadius.setComment("// 'radius' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

ColliderSphere::~ColliderSphere()
{
  destroy();
  ColliderSphere::shapeDestroy();
}

void
ColliderSphere::shapeInit(
  entt::registry& )
{
  CQDE_ASSERT_DEBUG(mShape == nullptr, return);
  mShape = mCommon->createSphereShape(1.0);
}

void
ColliderSphere::shapeDestroy()
{
  if ( mShape == nullptr )
    return;

  mCommon->destroySphereShape(mShape);
  mShape = nullptr;
}

Json::Value
ColliderSphere::shapeSerialize() const
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return Json::objectValue);

  Json::Value json {};

  json["radius"] = mShape->getRadius();

  return json;
}

void
ColliderSphere::shapeDeserialize(
  entt::registry&,
  const Json::Value& json )
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return);

  jsonValidateObject(json, colliderSphereJsonReference);

  mShape->setRadius(json["radius"].asFloat());
}

rp3d::CollisionShape*
ColliderSphere::shape() const
{
  return mShape;
}

std::string
ColliderSphere::type() const
{
  return "SphereShape";
}

} // namespace cqde::types
