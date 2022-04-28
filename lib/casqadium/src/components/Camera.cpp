#include <cqde/components/Camera.hpp>
#include <cqde/components/Transform.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

Camera::Camera(
  const Projection& _projection,
  const float _fov,
  const glm::vec4& _viewport,
  const std::pair <float, float>& _zRange )
  : fov(_fov)
  , viewport(_viewport)
  , zRange(_zRange)
  , projectionType(_projection)
  , renderMode(RenderMode::Solid)
  , textureMode(TextureMode::Textured)
  , lightingMode(LightingMode::Diffuse)
  , zBuffer()
{}

glm::mat4
Camera::projMatrix() const
{
  return  projectionType == Projection::Perspective ?
          glm::perspectiveFovRH_ZO( fov,
                                    viewport.z, viewport.w,
                                    zRange.first, zRange.second )
        : glm::ortho( viewport.x, viewport.z,
                      viewport.y, viewport.w,
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
  factory.type(entt::hashed_string{"Camera"});
  factory.func <&Camera::serialize> ("serialize"_hs);
  factory.func <&Camera::deserialize> ("deserialize"_hs);
}

} // namespace cqde::compos
