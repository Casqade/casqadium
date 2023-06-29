#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/json_glm_quat.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

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
  root.setComment("// root must be a JSON object"s,
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
  using fmt::format;
  jsonValidateObject(json, transformJsonReference);

  auto& comp = registry.emplace_or_replace <Transform> (entity);

  try
  {
    comp.translation << json["translation"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'translation' parse error: {}",
              e.what()));
  }

  try
  {
    comp.orientation << json["orientation"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'orientation' parse error: {}",
              e.what()));
  }

  try
  {
    comp.scale << json["scale"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'scale' parse error: {}",
              e.what()));
  }

  try
  {
    comp.scaleWorld << json["scaleWorld"];
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("'scaleWorld' parse error: {}",
              e.what()));
  }
}

} // namespace cqde::compos

namespace cqde
{

void
SetTranslationWorld(
  const glm::vec3& origin,
  const entt::registry& registry,
  const entt::entity entity,
  compos::Transform& cTransform )
{
  cTransform.translation
    = ToLocalSpace(origin, registry,
                   entity, cTransform );
}

void
SetOrientationWorld(
  const glm::quat& orientation,
  const entt::registry& registry,
  const entt::entity entity,
  compos::Transform& cTransform )
{
  cTransform.orientation
    = ToLocalSpace(orientation, registry,
                   entity, cTransform );
}

void
SetScaleWorld(
  const glm::vec3& scale,
  const entt::registry& registry,
  const entt::entity entity,
  compos::Transform& cTransform )
{
  cTransform.scale
    = ( glm::inverse(GetParentMatrix(registry, entity, cTransform))
        * glm::translate(glm::mat4(1.0f), scale))[3];
}

glm::vec3
ToLocalSpace(
  const glm::vec3& point,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return glm::translate(glm::inverse(GetParentMatrix(registry, entity, cTransform)),
                        point)[3];
}

glm::quat
ToLocalSpace(
  const glm::quat& orientation,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return glm::normalize( glm::quat(glm::inverse(GetParentMatrix(registry, entity, cTransform)))
                         * orientation );
}

glm::mat4
ToLocalSpace(
  const glm::mat4& matrix,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return  glm::inverse(GetParentMatrix(registry, entity, cTransform)) *
          matrix;
}


glm::vec3
ToWorldSpace(
  const glm::vec3& point,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return glm::translate( GetParentMatrix(registry, entity, cTransform),
                         point )[3];
}

glm::quat
ToWorldSpace(
  const glm::quat& orientation,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return glm::normalize(  glm::quat(GetParentMatrix(registry, entity, cTransform))
                          * orientation );
}

glm::mat4
ToWorldSpace(
  const glm::mat4& matrix,
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return  GetParentMatrix(registry, entity, cTransform)
          * matrix;
}


glm::mat4
GetDeltaMatrix(
  const glm::mat4& mat1,
  const glm::mat4& mat2 )
{
  return glm::inverse(mat1) * mat2;
}

glm::mat4
GetParentMatrix(
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  using compos::SceneNode;
  using compos::Transform;

  const std::function <glm::mat4(const SceneNode&)> parentModelWorld =
  [&] ( const SceneNode& child ) -> glm::mat4
  {
    const entt::entity eParent = child.parent.get(registry);

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

  const auto cSceneNode = registry.try_get <const SceneNode> (entity);

  return cSceneNode != nullptr
        ? parentModelWorld(*cSceneNode)
        : glm::mat4 {1.0f};
}

glm::mat4
GetWorldMatrix(
  const entt::registry& registry,
  const entt::entity entity,
  const compos::Transform& cTransform )
{
  return  GetParentMatrix(registry, entity, cTransform)
          * cTransform.modelLocal();
}

} // namespace cqde
