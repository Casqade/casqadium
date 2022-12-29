#include <cqde/types/physics/colliders/ColliderCapsule.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/collision/shapes/CapsuleShape.h>


namespace cqde::types
{

const static Json::Value colliderCapsuleJsonReference =
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

  auto& jsonHeight = root["height"];
  jsonHeight = ValueType::realValue;
  jsonHeight.setComment("// 'height' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

ColliderCapsule::~ColliderCapsule()
{
  destroy();
  ColliderCapsule::shapeDestroy();
}

void
ColliderCapsule::shapeInit(
  entt::registry& )
{
  CQDE_ASSERT_DEBUG(mShape == nullptr, return);
  mShape = mCommon->createCapsuleShape(1.0, 1.0);
}

void
ColliderCapsule::shapeDestroy()
{
  if ( mShape == nullptr )
    return;

  mCommon->destroyCapsuleShape(mShape);
  mShape = nullptr;
}

Json::Value
ColliderCapsule::shapeSerialize() const
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return Json::objectValue);

  Json::Value json {};

  json["height"] = mShape->getHeight();
  json["radius"] = mShape->getRadius();

  return json;
}

void
ColliderCapsule::shapeDeserialize(
  entt::registry&,
  const Json::Value& json )
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return);

  jsonValidateObject(json, colliderCapsuleJsonReference);

  mShape->setHeight(json["height"].asFloat());
  mShape->setRadius(json["radius"].asFloat());
}

rp3d::CollisionShape*
ColliderCapsule::shape() const
{
  return mShape;
}

std::string
ColliderCapsule::type() const
{
  return "CapsuleShape";
}

} // namespace cqde::types
