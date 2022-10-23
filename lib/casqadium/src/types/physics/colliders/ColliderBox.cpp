#include <cqde/types/physics/colliders/ColliderBox.hpp>

#include <cqde/common.hpp>
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

  auto& jsonShape = root["shape"];
  jsonShape = ValueType::objectValue;
  jsonShape.setComment("// 'shape' must be a JSON object"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

ColliderBox::ColliderBox()
{
  mState["type"] = ColliderBox::type();
  mState["shape"]["halfExtents"] << glm::vec3{1.0f};
}

ColliderBox::~ColliderBox()
{
  destroy();
  ColliderBox::shapeDisable();
}

void
ColliderBox::shapeEnable()
{
  if ( mShape != nullptr )
    return;

  CQDE_ASSERT_DEBUG(mCommon != nullptr, return);
  mShape = mCommon->createBoxShape(glmToRp3d(glm::vec3{1.0f}));
}

void
ColliderBox::shapeDisable()
{
  if ( mShape == nullptr )
    return;

  CQDE_ASSERT_DEBUG(mCommon != nullptr, return);

  mCommon->destroyBoxShape(mShape);
  mShape = nullptr;
}

void
ColliderBox::shapeStateApply()
{
  if ( mShape == nullptr )
    return;

  glm::vec3 halfExtents {};

  halfExtents << mState["shape"]["halfExtents"];

  mShape->setHalfExtents(glmToRp3d(halfExtents));
}

void
ColliderBox::shapeStateValidate()
{
  using fmt::format;

  jsonValidateObject(mState, colliderBoxJsonReference);

  try
  {
    glm::vec3 halfExtents {};
    halfExtents << mState["shape"]["halfExtents"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'halfExtents' parse error: {}",
              e.what()));
  }
}

Json::Value
ColliderBox::serialize() const
{
  auto json = Collider::serialize();

  if ( mShape == nullptr )
    return json;

  auto& jsonHalfExtents = json["shape"]["halfExtents"];

  jsonHalfExtents.clear();
  jsonHalfExtents << rp3dToGlm(mShape->getHalfExtents());

  return json;
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
