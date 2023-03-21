#include <cqde/components/physics/RigidBody.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/conversion/rp3d_glm.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>

#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/physics/ColliderFactory.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/body/RigidBody.h>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{


const static Json::Value rigidBodyJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& body = root["body"];
  body = ValueType::objectValue;
  body.setComment("// 'body' must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& colliders = root["colliders"];
  colliders = Json::arrayValue;
  colliders.setComment("// 'colliders' must be a JSON array"s,
                        Json::CommentPlacement::commentBefore);

  auto& collider = colliders.append(ValueType::objectValue);
  collider.setComment("// 'colliders' element must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  auto& colliderShape = collider["shape"];
  colliderShape = Json::objectValue;
  colliderShape.setComment("// collider 'shape' must be a JSON object"s,
                            Json::CommentPlacement::commentBefore);

  auto& colliderType = collider["type"];
  colliderType = Json::stringValue;
  colliderType.setComment("// collider 'type' must be a JSON string"s,
                          Json::CommentPlacement::commentBefore);


  auto& active = body["active"];
  active = ValueType::booleanValue;
  active.setComment("// body 'active' must be a JSON boolean"s,
                    Json::CommentPlacement::commentBefore);

  auto& sleeping = body["sleeping"];
  sleeping = ValueType::booleanValue;
  sleeping.setComment("// body 'sleeping' must be a JSON boolean"s,
                      Json::CommentPlacement::commentBefore);

  auto& sleepAllowed = body["sleepAllowed"];
  sleepAllowed = ValueType::booleanValue;
  sleepAllowed.setComment("// body 'sleepAllowed' must be a JSON boolean"s,
                          Json::CommentPlacement::commentBefore);

  auto& type = body["type"];
  type = ValueType::stringValue;
  type.setComment("// body 'type' must be a JSON string"s,
                  Json::CommentPlacement::commentBefore);


  auto& massPropsFromColliders = body["massPropsFromColliders"];
  massPropsFromColliders = ValueType::booleanValue;
  massPropsFromColliders.setComment("// body 'massPropsFromColliders' must be a JSON boolean"s,
                                    Json::CommentPlacement::commentBefore);

  auto& mass = body["mass"];
  mass = ValueType::realValue;
  mass.setComment("// body 'mass' must be a JSON float"s,
                  Json::CommentPlacement::commentBefore);


  auto& damp = body["damp"];
  damp = ValueType::objectValue;
  damp.setComment("// body 'damp' must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& dampLinear = damp["linear"];
  dampLinear = ValueType::realValue;
  dampLinear.setComment("// 'linear' body damp must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  auto& dampAngular = damp["angular"];
  dampAngular = ValueType::realValue;
  dampAngular.setComment("// 'angular' body damp must be a JSON float"s,
                          Json::CommentPlacement::commentBefore);


  auto& axisFactor = body["axisFactor"];
  axisFactor = ValueType::objectValue;
  axisFactor.setComment("// body 'axisFactor' must be a JSON object"s,
                        Json::CommentPlacement::commentBefore);


  auto& velocity = body["velocity"];
  velocity = ValueType::objectValue;
  velocity.setComment("// body 'velocity' must be a JSON object"s,
                      Json::CommentPlacement::commentBefore);


  auto& force = body["force"];
  force = ValueType::arrayValue;
  force.setComment("// body 'force' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  auto& torque = body["torque"];
  torque = ValueType::arrayValue;
  torque.setComment("// body 'torque' must be a JSON array"s,
                    Json::CommentPlacement::commentBefore);

  auto& inertia = body["inertia"];
  inertia = ValueType::arrayValue;
  inertia.setComment("// body 'inertia' must be a JSON array"s,
                      Json::CommentPlacement::commentBefore);

  return root;
}();

std::string
RigidBody::BodyTypeToString(
  const rp3d::BodyType type )
{
  using BodyType = rp3d::BodyType;

  switch (type)
  {
    case BodyType::STATIC:
      return "static";

    case BodyType::KINEMATIC:
      return "kinematic";

    case BodyType::DYNAMIC:
      return "dynamic";
  }

  CQDE_ASSERT_DEBUG(false, return "");
}

rp3d::BodyType
RigidBody::BodyTypeFromString(
  const std::string& type )
{
  using fmt::format;
  using BodyType = rp3d::BodyType;

  if ( type == BodyTypeToString(BodyType::STATIC) )
    return BodyType::STATIC;

  if ( type == BodyTypeToString(BodyType::KINEMATIC) )
    return BodyType::KINEMATIC;

  if ( type == BodyTypeToString(BodyType::DYNAMIC) )
    return BodyType::DYNAMIC;

  throw std::runtime_error(
    format("'{}' is not a valid RigidBody type", type));
}

RigidBody::RigidBody(
  RigidBody&& other )
{
  *this = std::move(other);
}

RigidBody&
RigidBody::operator = (
  RigidBody&& other )
{
  colliders = std::move(other.colliders);
  world = other.world;
  body = other.body;
  transformPrevious = other.transformPrevious;

  if ( body != nullptr )
    body->setUserData(this);

  other.body = nullptr;

  return *this;
}

RigidBody::~RigidBody()
{
  colliders.clear();

  if ( body == nullptr )
    return;

  world->destroyRigidBody(body);
  body = nullptr;
}

Json::Value
RigidBody::serialize() const
{
  Json::Value json {};

  auto& jsonBody = json["body"];

  jsonBody["massPropsFromColliders"] = massPropsFromColliders;

  if ( body != nullptr )
  {
    jsonBody["active"] = body->isActive();
    jsonBody["sleeping"] = body->isSleeping();
    jsonBody["sleepAllowed"] = body->isAllowedToSleep();
    jsonBody["type"] = BodyTypeToString(body->getType());
    jsonBody["mass"] = body->getMass();

    jsonBody["damp"]["linear"] = body->getLinearDamping();
    jsonBody["damp"]["angular"] = body->getAngularDamping();

    jsonBody["axisFactor"]["linear"].clear();
    jsonBody["axisFactor"]["linear"] << rp3dToGlm(body->getLinearLockAxisFactor());

    jsonBody["axisFactor"]["angular"].clear();
    jsonBody["axisFactor"]["angular"] << rp3dToGlm(body->getAngularLockAxisFactor());

    jsonBody["velocity"]["linear"].clear();
    jsonBody["velocity"]["linear"] << rp3dToGlm(body->getLinearVelocity());

    jsonBody["velocity"]["angular"].clear();
    jsonBody["velocity"]["angular"] << rp3dToGlm(body->getAngularVelocity());

    jsonBody["force"].clear();
    jsonBody["force"] << rp3dToGlm(body->getForce());

    jsonBody["torque"].clear();
    jsonBody["torque"] << rp3dToGlm(body->getTorque());

    jsonBody["inertia"].clear();
    jsonBody["inertia"] << rp3dToGlm(body->getLocalInertiaTensor());

    jsonBody["massCenter"].clear();
    jsonBody["massCenter"] << rp3dToGlm(body->getLocalCenterOfMass());
  }
  else
  {
    jsonBody["active"] = false;
    jsonBody["sleeping"] = false;
    jsonBody["sleepAllowed"] = true;
    jsonBody["type"] = BodyTypeToString(rp3d::BodyType::STATIC);
    jsonBody["mass"] = 1.0f;
    jsonBody["massPropsFromColliders"] = massPropsFromColliders;

    auto& jsonDamp = jsonBody["damp"];
    jsonDamp["linear"] = Json::realValue;
    jsonDamp["angular"] = Json::realValue;

    auto& jsonAxisFactor = jsonBody["axisFactor"];
    jsonAxisFactor["linear"] << glm::vec3{1.0f};
    jsonAxisFactor["angular"] << glm::vec3{1.0f};

    auto& jsonVelocity = jsonBody["velocity"];
    jsonVelocity["linear"] << glm::vec3{};
    jsonVelocity["angular"] << glm::vec3{};

    jsonBody["force"] << glm::vec3{};
    jsonBody["torque"] << glm::vec3{};
    jsonBody["inertia"] << glm::vec3{};
    jsonBody["massCenter"] << glm::vec3{};
  }

  auto& jsonColliders = json["colliders"];
  jsonColliders = Json::arrayValue;

  for ( const auto& collider : colliders )
    jsonColliders.append(collider->serialize());

  return json;
}

void
RigidBody::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using fmt::format;
  using types::PhysicsManager;
  using types::ColliderFactory;

  jsonValidateObject(json, rigidBodyJsonReference);

  auto& comp = registry.emplace_or_replace <RigidBody> (entity);

  comp.world = registry.ctx().get <PhysicsManager> ().world();

  comp.body = comp.world->createRigidBody({});
  comp.body->setUserData(&comp);

  auto& jsonBody = json["body"];

  comp.body->setType(BodyTypeFromString(jsonBody["type"].asString()));

  comp.body->setLinearDamping(jsonBody["damp"]["linear"].asFloat());
  comp.body->setAngularDamping(jsonBody["damp"]["angular"].asFloat());


  glm::vec3 axisLockFactor {};

  axisLockFactor << jsonBody["axisFactor"]["linear"];
  comp.body->setLinearLockAxisFactor(glmToRp3d(axisLockFactor));

  axisLockFactor << jsonBody["axisFactor"]["angular"];
  comp.body->setAngularLockAxisFactor(glmToRp3d(axisLockFactor));


  const auto& colliderFactory = registry.ctx().get <ColliderFactory> ();
  const auto& jsonColliders = json["colliders"];

  for ( const auto& jsonCollider : jsonColliders )
  {
    const auto shapeId = jsonCollider["type"].asString();
    const auto collider = colliderFactory.create(shapeId);

    try
    {
      if ( collider == nullptr )
        throw std::runtime_error("Unknown collider type");

      collider->deserialize(registry, comp.body, jsonCollider);
    }
    catch ( const std::exception& e )
    {
      throw std::runtime_error(
        format("Failed to deserialize collider '{}': {}",
                shapeId, e.what()));
    }

    comp.colliders.push_back(collider);
  }


  comp.massPropsFromColliders = jsonBody["massPropsFromColliders"].asBool();

  if ( comp.massPropsFromColliders == true )
    comp.body->updateMassPropertiesFromColliders();
  else
  {
    glm::vec3 massCenter {};
    massCenter << jsonBody["massCenter"];
    comp.body->setLocalCenterOfMass(glmToRp3d(massCenter));

    glm::vec3 inertia {};
    inertia << jsonBody["inertia"];
    comp.body->setLocalInertiaTensor(glmToRp3d(inertia));

    comp.body->setMass(jsonBody["mass"].asFloat());
  }

  comp.body->setIsSleeping(jsonBody["sleeping"].asBool());
  comp.body->setIsAllowedToSleep(jsonBody["sleepAllowed"].asBool());
  comp.body->setIsActive(jsonBody["active"].asBool());

  glm::vec3 force {};
  force << jsonBody["force"];
  comp.body->applyWorldForceAtCenterOfMass(glmToRp3d(force));

  glm::vec3 torque {};
  torque << jsonBody["torque"];
  comp.body->applyWorldTorque(glmToRp3d(torque));


  glm::vec3 velocity {};

  velocity << jsonBody["velocity"]["linear"];
  comp.body->setLinearVelocity(glmToRp3d(velocity));

  velocity << jsonBody["velocity"]["angular"];
  comp.body->setAngularVelocity(glmToRp3d(velocity));
}

} // namespace cqde::compos
