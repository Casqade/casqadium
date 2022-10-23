#include <cqde/components/physics/CollisionBody.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/types/EntityReference.hpp>
#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/physics/ColliderFactory.hpp>

#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>

#include <reactphysics3d/engine/PhysicsWorld.h>
#include <reactphysics3d/body/CollisionBody.h>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::compos
{

const static Json::Value collisionBodyJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& jsonBody = root["body"];
  jsonBody = ValueType::objectValue;
  jsonBody.setComment("// 'body' must be a JSON object"s,
                      Json::CommentPlacement::commentBefore);

  auto& jsonColliders = root["colliders"];
  jsonColliders = Json::arrayValue;
  jsonColliders.setComment("// 'colliders' must be a JSON array"s,
                            Json::CommentPlacement::commentBefore);

  auto& jsonCollider = jsonColliders.append(ValueType::objectValue);
  jsonCollider.setComment("// 'colliders' element must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  auto& jsonColliderShape = jsonCollider["shape"];
  jsonColliderShape = Json::objectValue;
  jsonColliderShape.setComment("// collider 'shape' must be a JSON object"s,
                                Json::CommentPlacement::commentBefore);

  auto& jsonColliderType = jsonCollider["type"];
  jsonColliderType = Json::stringValue;
  jsonColliderType.setComment("// collider 'type' must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

CollisionBody::CollisionBody()
{
  bodyState["active"] = false;
}

CollisionBody::~CollisionBody()
{
  colliders.clear();

  if ( body != nullptr &&
       world != nullptr )
    world->destroyCollisionBody(body);

  body = nullptr;
}

void
CollisionBody::enable(
  entt::registry& registry )
{
  using types::PhysicsManager;

  if ( body != nullptr )
    return;

  if ( world == nullptr )
    world = registry.ctx().at <PhysicsManager> ().world();

  body = world->createCollisionBody({});
  body->setUserData(this);

  for ( auto& collider : colliders )
    collider->enable(registry, body);

  body->setIsActive(bodyState["active"].asBool());
}

void
CollisionBody::disable()
{
  if ( body == nullptr )
    return;

  CQDE_ASSERT_DEBUG(world != nullptr, return);

  bodyState["active"] = body->isActive();

  for ( auto& collider : colliders )
    collider->disable();

  world->destroyCollisionBody(body);
  body = nullptr;
}

Json::Value
CollisionBody::serialize() const
{
  Json::Value json {};

  auto& jsonBodyState = json["body"];

  if ( body != nullptr )
    jsonBodyState["active"] = body->isActive();
  else
    jsonBodyState = bodyState;


  auto& jsonColliders = json["colliders"];
  jsonColliders = Json::arrayValue;

  for ( const auto& collider : colliders )
    jsonColliders.append(collider->serialize());

  return json;
}

void
CollisionBody::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using fmt::format;
  using types::ColliderFactory;

  jsonValidateObject(json, collisionBodyJsonReference);

  auto& comp = registry.emplace_or_replace <CollisionBody> (entity);

  comp.bodyState = json["body"];

  const auto& colliderFactory = registry.ctx().at <ColliderFactory> ();
  const auto& jsonColliders = json["colliders"];

  for ( const auto& jsonCollider : jsonColliders )
  {
    const auto shapeId = jsonCollider["type"].asString();
    const auto collider = colliderFactory.create(shapeId);

    try
    {
      collider->deserialize(jsonCollider);
    }
    catch ( const std::exception& e )
    {
      throw std::runtime_error(
        format("Failed to deserialize collider '{}': {}",
                shapeId, e.what()));
    }

    comp.colliders.push_back(collider);
  }
}

} // namespace cqde::compos
