#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

Camera::Camera(
  const glm::vec4& _viewport,
  const Projection _projectionType )
  : viewport(_viewport)
  , projectionType(_projectionType)
{}

glm::mat4
Camera::projMatrix() const
{
  return  projectionType == Projection::Perspective ?
          glm::perspectiveFov(fov,
                              viewport.z, viewport.w,
                              zRange.first, zRange.second )
        : glm::ortho( -viewport.z * 0.5f, viewport.z * 0.5f,
                      -viewport.w * 0.5f, viewport.w * 0.5f,
                      zRange.first, zRange.second );
}

glm::mat4
Camera::viewMatrix(
  entt::registry&   registry,
  const SceneNode&  cSceneNode,
  const Transform&  cTransform ) const
{
  return glm::inverse(GetWorldMatrix(registry, cTransform, cSceneNode));
}

void
Camera::serialize( Json::Value& json ) const
{}

void
Camera::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <Camera> (entity);
}

void
Camera::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <Camera> ();
  factory.type("Camera"_hs);
  factory.func <&Camera::serialize> ("serialize"_hs);
  factory.func <&Camera::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
