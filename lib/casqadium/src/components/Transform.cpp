#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/json_glm_quat.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

Transform::Transform(
  const glm::vec3& _translation,
  const glm::quat& _orientation )
  : translation(_translation)
  , orientation(_orientation)
{}

glm::mat4
Transform::modelLocal() const
{
  return
    glm::translate(glm::mat4(1.0f), translation) *
    glm::toMat4(orientation) *
    glm::scale(glm::mat4(1.0f), scale) *
    glm::scale(glm::mat4(1.0f), scaleWorld);
}

glm::vec3
Transform::front() const
{
  return glm::rotate( orientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Transform::right() const
{
  return glm::rotate( orientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Transform::up() const
{
  return glm::rotate( orientation, {0.0f, 1.0f, 0.0f} );
}

const static Json::Value transformJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// JSON root must be an object"s,
                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
Transform::serialize() const
{
  Json::Value json {};

  json["translation"] << translation;
  json["orientation"] << orientation;
  json["scale"] << scale;
  json["scaleWorld"] << scaleWorld;

  return json;
}

void
Transform::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  jsonValidateObject(json, transformJsonReference);

  auto& comp = registry.emplace <Transform> (entity);

  comp.translation << json["translation"];
  comp.orientation << json["orientation"];
  comp.scale << json["scale"];
  comp.scaleWorld << json["scaleWorld"];
}

} // namespace cqde::compos

namespace cqde
{

void
SetTranslationWorld(
  const glm::vec3& origin,
  const entt::registry& registry,
  compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  cTransform.translation = ToLocalSpace(origin,
                                        registry,
                                        cTransform,
                                        cSceneNode );
}

void
SetOrientationWorld(
  const glm::quat& orientation,
  const entt::registry& registry,
  compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  cTransform.orientation = ToLocalSpace(orientation,
                                        registry,
                                        cTransform,
                                        cSceneNode );
}

void
SetScaleWorld(
  const glm::vec3& scale,
  const entt::registry& registry,
  compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  cTransform.scale = (cTransform.modelLocal() *
                      glm::inverse(GetWorldMatrix(registry,
                                                  cTransform,
                                                  cSceneNode)) *
                      glm::translate(glm::mat4(1.0f), scale))[3];
}

glm::vec3
ToLocalSpace(
  const glm::vec3& point,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return glm::translate( cTransform.modelLocal() *
                         glm::inverse(GetWorldMatrix(registry,
                                                     cTransform,
                                                     cSceneNode)),
                         point )[3];
}

glm::quat
ToLocalSpace(
  const glm::quat& orientation,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return glm::normalize( glm::quat(cTransform.modelLocal() *
                                   glm::inverse(GetWorldMatrix(registry,
                                                               cTransform,
                                                               cSceneNode))) *
                         orientation );
}

glm::mat4
ToLocalSpace(
  const glm::mat4& matrix,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return  cTransform.modelLocal() *
          glm::inverse(GetWorldMatrix(registry,
                                      cTransform,
                                      cSceneNode)) *
          matrix;
}


glm::vec3 ToWorldSpace(
  const glm::vec3& point,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return glm::translate( GetWorldMatrix(registry,
                                        cTransform,
                                        cSceneNode) *
                         glm::inverse(cTransform.modelLocal()),
                         point )[3];
}

glm::quat ToWorldSpace(
  const glm::quat& orientation,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return glm::normalize(  glm::quat(GetWorldMatrix( registry,
                                                    cTransform,
                                                    cSceneNode) *
                                    glm::inverse(cTransform.modelLocal())) *
                          orientation );
}

glm::mat4
ToWorldSpace(
  const glm::mat4& matrix,
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return  GetWorldMatrix( registry,
                          cTransform,
                          cSceneNode) *
          glm::inverse(cTransform.modelLocal()) *
          matrix;
}

glm::mat4
GetWorldMatrix(
  const entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  using compos::SceneNode;
  using compos::Transform;

  const std::function <glm::mat4(const SceneNode&)> parentModelWorld =
  [&] ( const SceneNode& child ) -> glm::mat4
  {
    const entt::entity eParent = child.parent.get_if_valid(registry);

    if ( eParent == entt::null )
      return glm::mat4{1.0f};

    const auto [parentTransform, parentSceneNode] = registry.try_get <const Transform, const SceneNode> (eParent);

    if ( parentSceneNode == nullptr ||
         parentTransform == nullptr )
      return glm::mat4{1.0f};

    return
    {
      parentModelWorld(*parentSceneNode) *
      glm::translate(glm::mat4(1.0f), parentTransform->translation) *
      glm::toMat4(parentTransform->orientation) *
      glm::scale(glm::mat4(1.0f), parentTransform->scaleWorld)
    };
  };

  return parentModelWorld(cSceneNode) * cTransform.modelLocal();
}

} // namespace cqde
