#include <cqde/types/physics/colliders/ColliderBox.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <glm/mat4x4.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>

#include <spdlog/fmt/bundled/format.h>


namespace cqde::types
{

const static Json::Value colliderBoxJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& jsonHalfExtents = root["halfExtents"];
  jsonHalfExtents = ValueType::arrayValue;
  jsonHalfExtents.setComment("// 'halfExtents' must be a JSON array"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

ColliderBox::~ColliderBox()
{
  destroy();
  ColliderBox::shapeDestroy();
}

void
ColliderBox::shapeInit(
  entt::registry& )
{
  CQDE_ASSERT_DEBUG(mShape == nullptr, return);
  mShape = mCommon->createBoxShape(glmToRp3d(glm::vec3{1.0f}));
}

void
ColliderBox::shapeDestroy()
{
  if ( mShape == nullptr )
    return;

  mCommon->destroyBoxShape(mShape);
  mShape = nullptr;
}

Json::Value
ColliderBox::shapeSerialize() const
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return Json::objectValue);

  Json::Value json {};

  auto& jsonHalfExtents = json["halfExtents"];

  jsonHalfExtents.clear();
  jsonHalfExtents << rp3dToGlm(mShape->getHalfExtents());

  return json;
}

void
ColliderBox::shapeDeserialize(
  entt::registry&,
  const Json::Value& json )
{
  using fmt::format;

  CQDE_ASSERT_DEBUG(mShape != nullptr, return);

  jsonValidateObject(json, colliderBoxJsonReference);

  glm::vec3 halfExtents {};

  try
  {
    halfExtents << json["halfExtents"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'halfExtents' parse error: {}",
              e.what()));
  }

  mShape->setHalfExtents(glmToRp3d(halfExtents));
}

rp3d::CollisionShape*
ColliderBox::shape() const
{
  return mShape;
}

std::string
ColliderBox::type() const
{
  return "BoxShape";
}

} // namespace cqde::types
