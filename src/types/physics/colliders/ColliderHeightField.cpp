#include <cqde/types/physics/colliders/ColliderHeightField.hpp>

#include <cqde/assert.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/types/assets/TerrainAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <glm/mat4x4.hpp>

#include <reactphysics3d/engine/PhysicsCommon.h>
#include <reactphysics3d/collision/shapes/HeightFieldShape.h>

#include <spdlog/fmt/bundled/format.h>


namespace cqde::types
{

const static Json::Value colliderHeightFieldJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& jsonTerrainId = root["terrainId"];
  jsonTerrainId = ValueType::stringValue;
  jsonTerrainId.setComment("// 'terrainId' must be a JSON string"s,
                          Json::CommentPlacement::commentBefore);

  auto& jsonScale = root["scale"];
  jsonScale = ValueType::arrayValue;
  jsonScale.setComment("// 'scale' must be a JSON array"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

ColliderHeightField::~ColliderHeightField()
{
  destroy();
  ColliderHeightField::shapeDestroy();
}

void
ColliderHeightField::shapeInit(
  entt::registry& registry )
{
  using HeightDataType = rp3d::HeightFieldShape::HeightDataType;

  CQDE_ASSERT_DEBUG(mShape == nullptr, return);

  const auto terrain = registry.ctx().get <TerrainAssetManager> ().try_get({});

  mShape = mCommon->createHeightFieldShape(2, 2, 0.0, 0.0,
                                           terrain->data.data(),
                                           HeightDataType::HEIGHT_FLOAT_TYPE);
}

void
ColliderHeightField::shapeDestroy()
{
  if ( mShape == nullptr )
    return;

  mCommon->destroyHeightFieldShape(mShape);
  mShape = nullptr;
}

Json::Value
ColliderHeightField::shapeSerialize() const
{
  CQDE_ASSERT_DEBUG(mShape != nullptr, return Json::objectValue);

  Json::Value json {};

  json["terrainId"] = mTerrainId.str();
  json["scale"] << rp3dToGlm(mShape->getScale());

  return json;
}

void
ColliderHeightField::shapeDeserialize(
  entt::registry& registry,
  const Json::Value& json )
{
  using fmt::format;

  CQDE_ASSERT_DEBUG(mShape != nullptr, return);

  jsonValidateObject(json, colliderHeightFieldJsonReference);

  glm::vec3 scale {};

  try
  {
    scale << json["scale"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'scale' parse error: {}",
              e.what()));
  }

  mShape->setScale(glmToRp3d(scale));

  mTerrainId = json["terrainId"].asString();

  const auto& terrainManager = registry.ctx().get <TerrainAssetManager> ();

  const auto terrain = terrainManager.try_get(mTerrainId);

  if ( terrain == nullptr )
  {
    throw std::runtime_error(
      format("terrain '{}' must be loaded before {} construction",
             mTerrainId.str(), ColliderHeightField::type()));
  }

  mShape->setMinMaxHeight(terrain->heightRange.first,
                          terrain->heightRange.second);

  mShape->setNbColumns(terrain->grid.columns);
  mShape->setNbRows(terrain->grid.rows);

  mShape->setHeightFieldData(terrain->data.data());

  mShape->computeLocalAABB();
}

rp3d::CollisionShape*
ColliderHeightField::shape() const
{
  return mShape;
}

std::string
ColliderHeightField::type() const
{
  return "HeightFieldShape";
}

} // namespace cqde::types
