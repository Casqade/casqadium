#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <entt/entt.hpp>

#include <glm/mat4x4.hpp>


namespace cqde::compos
{

Transform::Transform(
  const glm::vec3& _translation,
  const glm::quat& _orientation,
  const glm::vec3& _scale,
  const glm::vec3& _scaleWorld )
  : translation(_translation)
  , orientation(_orientation)
  , scale(_scale)
  , scaleWorld(_scaleWorld)
{}

glm::mat4
Transform::modelLocal() const
{
  return
    glm::translate(glm::mat4(1.0f), translation) *
    glm::toMat4(orientation) *
    glm::scale(glm::mat4(1.0f), scale);
}

glm::vec3
Transform::front() const
{
  return glm::rotate( orientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Transform::right() const
{
  return glm::rotate(  orientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Transform::up() const
{
  return glm::rotate( orientation, {0.0f, 1.0f, 0.0f} );
}

void
Transform::serialize( const std::string& json ) const
{}

void
Transform::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <Transform> (entity);
}

void
Transform::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <Transform> ();
  factory.type(entt::hashed_string{"Transform"});
  factory.data <&Transform::translation> ("translation"_hs);
  factory.data <&Transform::orientation> ("orientation"_hs);
  factory.data <&Transform::scale> ("scale"_hs);
  factory.data <&Transform::scaleWorld> ("scaleWorld"_hs);
  factory.func <&Transform::modelLocal> ("modelLocal"_hs);
  factory.func <&Transform::front> ("front"_hs);
  factory.func <&Transform::right> ("right"_hs);
  factory.func <&Transform::up> ("up"_hs);
  factory.func <&Transform::serialize> ("serialze"_hs);
  factory.func <&Transform::deserialize> ("deserialze"_hs);
}

} // namespace cqde::compos

namespace cqde
{

void
SetOriginWorld(
  const glm::vec3& origin,
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
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
  entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  return GetWorldMatrix(registry,
                        cTransform,
                        cSceneNode) *
                        glm::inverse(cTransform.modelLocal()) *
                        matrix;
}

glm::mat4
GetWorldMatrix(
  entt::registry& registry,
  const compos::Transform& cTransform,
  const compos::SceneNode& cSceneNode )
{
  using compos::SceneNode;
  using compos::Transform;

  std::function <glm::mat4(const SceneNode&)> parentModelWorld =
  [&] ( const SceneNode& child ) -> glm::mat4
  {
    entt::entity eParent = child.parent.get(registry);

    if ( registry.valid(eParent) == false )
      return glm::mat4(1.0f);

    const auto& [parentTransform, parentSceneNode] = registry.get <Transform, SceneNode> (eParent);

    return
    {
      parentModelWorld(parentSceneNode) *
      glm::translate(glm::mat4(1.0f), parentTransform.translation) *
      glm::toMat4(parentTransform.orientation) *
      glm::scale(glm::mat4(1.0f), parentTransform.scaleWorld)
    };
  };

  return parentModelWorld(cSceneNode) * cTransform.modelLocal();
}

} // namespace cqde
