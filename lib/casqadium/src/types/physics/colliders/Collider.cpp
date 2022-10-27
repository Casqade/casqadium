#include <cqde/types/physics/colliders/Collider.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/conversion/json_glm_vec4.hpp>

#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <reactphysics3d/collision/Collider.h>
#include <reactphysics3d/engine/EventListener.h>

#include <entt/entity/registry.hpp>

#include <glm/mat4x4.hpp>

#include <spdlog/fmt/bundled/format.h>


namespace cqde::types
{

const static Json::Value colliderJsonReference =
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

  auto& jsonCallbacks = root["callbacks"];
  jsonCallbacks = ValueType::objectValue;
  jsonCallbacks.setComment("// 'callbacks' must be a JSON object"s,
                            Json::CommentPlacement::commentBefore);

  auto& jsonCallbacksOnEnter = jsonCallbacks["onEnter"];
  jsonCallbacksOnEnter = Json::arrayValue;
  jsonCallbacksOnEnter.setComment("// callbacks 'onEnter' must be a JSON array"s,
                                  Json::CommentPlacement::commentBefore);

  jsonCallbacksOnEnter.append(ValueType::stringValue);
  jsonCallbacksOnEnter.begin()->setComment("// callbacks 'onEnter' element must be a JSON string"s,
                                            Json::CommentPlacement::commentBefore);

  auto& jsonCallbacksOnStay = jsonCallbacks["onStay"];
  jsonCallbacksOnStay = Json::arrayValue;
  jsonCallbacksOnStay.setComment("// callbacks 'onStay' must be a JSON array"s,
                                  Json::CommentPlacement::commentBefore);

  jsonCallbacksOnStay.append(ValueType::stringValue);
  jsonCallbacksOnStay.begin()->setComment("// callbacks 'onStay' element must be a JSON string"s,
                                        Json::CommentPlacement::commentBefore);

  auto& jsonCallbacksOnLeave = jsonCallbacks["onLeave"];
  jsonCallbacksOnLeave = Json::arrayValue;
  jsonCallbacksOnLeave.setComment("// callbacks 'onLeave' must be a JSON array"s,
                                  Json::CommentPlacement::commentBefore);

  jsonCallbacksOnLeave.append(ValueType::stringValue);
  jsonCallbacksOnLeave.begin()->setComment("// callbacks 'onLeave' element must be a JSON string"s,
                                            Json::CommentPlacement::commentBefore);


  auto& jsonTrigger = root["isTrigger"];
  jsonTrigger = ValueType::booleanValue;
  jsonTrigger.setComment("// 'isTrigger' must be a JSON boolean"s,
                          Json::CommentPlacement::commentBefore);


  auto& jsonCollision = root["collision"];
  jsonCollision = ValueType::objectValue;
  jsonCollision.setComment("// 'collision' must be a JSON object"s,
                            Json::CommentPlacement::commentBefore);

  auto& jsonCollisionGroup = jsonCollision["group"];
  jsonCollisionGroup = ValueType::uintValue;
  jsonCollisionGroup.setComment("// collision 'group' must be a JSON unsigned integer"s,
                                Json::CommentPlacement::commentBefore);

  auto& jsonCollisionMask = jsonCollision["mask"];
  jsonCollisionMask = ValueType::uintValue;
  jsonCollisionMask.setComment("// collision 'mask' must be a JSON unsigned integer"s,
                                Json::CommentPlacement::commentBefore);


  auto& jsonMaterial = root["material"];
  jsonMaterial = ValueType::objectValue;
  jsonMaterial.setComment("// 'material' must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  auto& jsonMaterialBounciness = jsonMaterial["bounciness"];
  jsonMaterialBounciness = ValueType::realValue;
  jsonMaterialBounciness.setComment("// material 'bounciness' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  auto& jsonMaterialDensity = jsonMaterial["density"];
  jsonMaterialDensity = ValueType::realValue;
  jsonMaterialDensity.setComment("// material 'density' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  auto& jsonMaterialFriction = jsonMaterial["friction"];
  jsonMaterialFriction = ValueType::realValue;
  jsonMaterialFriction.setComment("// material 'friction' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Collider::~Collider()
{
  destroy();
}

void
Collider::init(
  entt::registry& registry,
  rp3d::CollisionBody* body )
{
  CQDE_ASSERT_DEBUG(mCommon == nullptr, return);
  CQDE_ASSERT_DEBUG(mCollider == nullptr, return);

  auto& physicsManager = registry.ctx().at <PhysicsManager> ();
  mCommon = &physicsManager.common();

  shapeInit(registry);

  const bool active = body->isActive();
  body->setIsActive(true);

  mCollider = body->addCollider(shape(), {});
  mCollider->setUserData(static_cast <Collider*> (this));

  body->setIsActive(active);
}

void
Collider::shapeInit(
  entt::registry& )
{}

void
Collider::destroy()
{
  if ( mCollider == nullptr )
    return;

  const auto body = mCollider->getBody();
  if ( body != nullptr )
    body->removeCollider(mCollider);

  mCollider = nullptr;
}

void
Collider::shapeDestroy()
{}

void
Collider::onEnter(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  if ( mCallbacks.onEnter.empty() == true )
    return;

  const auto& callbackManager = registry.ctx().at <CallbackManager> ();

  for ( const auto& callback : mCallbacks.onEnter )
    callbackManager.execute(callback, registry, args);
}

void
Collider::onStay(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  if ( mCallbacks.onStay.empty() == true )
    return;

  const auto& callbackManager = registry.ctx().at <CallbackManager> ();

  for ( const auto& callback : mCallbacks.onStay )
    callbackManager.execute(callback, registry, args);
}

void
Collider::onLeave(
  entt::registry& registry,
  const std::vector <std::any>& args )
{
  if ( mCallbacks.onLeave.empty() == true )
    return;

  const auto& callbackManager = registry.ctx().at <CallbackManager> ();

  for ( const auto& callback : mCallbacks.onLeave )
    callbackManager.execute(callback, registry, args);
}

Json::Value
Collider::serialize() const
{
  CQDE_ASSERT_DEBUG(mCollider != nullptr, return Json::objectValue);

  Json::Value json {};

  json["type"] = type();

  auto& jsonCallbacks = json["callbacks"];
  jsonCallbacks.clear();

  auto& jsonCallbacksOnEnter = jsonCallbacks["onEnter"];
  jsonCallbacksOnEnter = Json::arrayValue;

  for ( const auto& callback : mCallbacks.onEnter )
    jsonCallbacksOnEnter.append(callback.str());

  auto& jsonCallbacksOnStay = jsonCallbacks["onStay"];
  jsonCallbacksOnStay = Json::arrayValue;

  for ( const auto& callback : mCallbacks.onStay )
    jsonCallbacksOnStay.append(callback.str());

  auto& jsonCallbacksOnLeave = jsonCallbacks["onLeave"];
  jsonCallbacksOnLeave = Json::arrayValue;

  for ( const auto& callback : mCallbacks.onLeave )
    jsonCallbacksOnLeave.append(callback.str());

  json["isTrigger"] = mCollider->getIsTrigger();

  auto& jsonTransform = json["transform"];
  const auto transform = rp3dToGlm(mCollider->getLocalToBodyTransform());

  jsonTransform.clear();
  jsonTransform[0] << transform[0];
  jsonTransform[1] << transform[1];
  jsonTransform[2] << transform[2];
  jsonTransform[3] << transform[3];

  auto& jsonCollision = json["collision"];
  jsonCollision["group"] = mCollider->getCollisionCategoryBits();
  jsonCollision["mask"] = mCollider->getCollideWithMaskBits();

  auto& jsonMaterial = json["material"];
  const auto& material = mCollider->getMaterial();

  jsonMaterial["bounciness"] = material.getBounciness();
  jsonMaterial["density"] = material.getMassDensity();
  jsonMaterial["friction"] = material.getFrictionCoefficient();

  json["shape"] = shapeSerialize();

  return json;
}

Json::Value
Collider::shapeSerialize() const
{
  return Json::objectValue;
}

void
Collider::deserialize(
  entt::registry& registry,
  rp3d::CollisionBody* body,
  const Json::Value& json )
{
  using fmt::format;

  CQDE_ASSERT_DEBUG(mCollider == nullptr, return);

  jsonValidateObject(json, colliderJsonReference);

  const auto& jsonCallbacks = json["callbacks"];
  const auto& jsonCollision = json["collision"];
  const auto& jsonMaterial = json["material"];
  const auto& jsonTransform = json["transform"];
  const auto& jsonTrigger = json["isTrigger"];

  mCallbacks.onEnter.clear();
  for ( const auto& callback : jsonCallbacks["onEnter"] )
    mCallbacks.onEnter.push_back(callback.asString());

  mCallbacks.onStay.clear();
  for ( const auto& callback : jsonCallbacks["onStay"] )
    mCallbacks.onStay.push_back(callback.asString());

  mCallbacks.onLeave.clear();
  for ( const auto& callback : jsonCallbacks["onLeave"] )
    mCallbacks.onLeave.push_back(callback.asString());

  glm::mat4 transform {1.0f};

  try
  {
    transform[0] << jsonTransform[0];
    transform[1] << jsonTransform[1];
    transform[2] << jsonTransform[2];
    transform[3] << jsonTransform[3];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'transform' parse error: {}",
              e.what()));
  }

  init(registry, body);

  shapeDeserialize(registry, json["shape"]);

  mCollider->setLocalToBodyTransform(glmToRp3d(transform));

  mCollider->setIsTrigger(jsonTrigger.asBool());

  mCollider->setCollisionCategoryBits(jsonCollision["group"].asUInt());
  mCollider->setCollideWithMaskBits(jsonCollision["mask"].asUInt());

  auto& material = mCollider->getMaterial();
  material.setBounciness(jsonMaterial["bounciness"].asFloat());
  material.setMassDensity(jsonMaterial["density"].asFloat());
  material.setFrictionCoefficient(jsonMaterial["friction"].asFloat());
}

void
Collider::shapeDeserialize(
  entt::registry&,
  const Json::Value& )
{}

rp3d::Collider*
Collider::collider() const
{
  return mCollider;
}

rp3d::CollisionShape*
Collider::shape() const
{
  return {};
}

std::string
Collider::type() const
{
  return "Collider";
}

} // namespace cqde::types
