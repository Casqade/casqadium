#include <cqde/components/physics/CollisionBody.hpp>

#include <cqde/common.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/types/PhysicsManager.hpp>
#include <cqde/types/physics/ColliderFactory.hpp>

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

  auto& body = root["body"];
  body = ValueType::objectValue;
  body.setComment("// 'body' must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  auto& active = body["active"];
  active = ValueType::booleanValue;
  active.setComment("// body 'active' must be a JSON boolean"s,
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

  return root;
}();

CollisionBody::CollisionBody(
  CollisionBody&& other )
{
  *this = std::move(other);
}

CollisionBody&
CollisionBody::operator = (
  CollisionBody&& other )
{
  colliders = std::move(other.colliders);
  world = other.world;
  body = other.body;

  if ( body != nullptr )
    body->setUserData(this);

  other.body = nullptr;

  return *this;
}

CollisionBody::~CollisionBody()
{
  colliders.clear();

  if ( body == nullptr )
    return;

  world->destroyCollisionBody(body);
  body = nullptr;
}

Json::Value
CollisionBody::serialize() const
{
  Json::Value json {};

  if ( body != nullptr )
    json["body"]["active"] = body->isActive();
  else
    json["body"]["active"] = false;

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
  using types::PhysicsManager;
  using types::ColliderFactory;

  jsonValidateObject(json, collisionBodyJsonReference);

  auto& comp = registry.emplace_or_replace <CollisionBody> (entity);

  comp.world = registry.ctx().get <PhysicsManager> ().world();

  comp.body = comp.world->createCollisionBody({});
  comp.body->setUserData(&comp);

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

  comp.body->setIsActive(json["body"]["active"].asBool());
}

} // namespace cqde::compos
