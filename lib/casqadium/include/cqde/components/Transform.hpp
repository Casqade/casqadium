#pragma once

#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

#include <json/forwards.h>


namespace cqde
{

namespace compos
{

struct Transform
{
  glm::vec3 translation {};
  glm::quat orientation = glm::vec3{};
  glm::vec3 scale {1.0f};
  glm::vec3 scaleWorld{1.0f};


  Transform() = default;
  Transform(  const glm::vec3& origin,
              const glm::quat& orientation = glm::vec3{} );

  glm::mat4 modelLocal() const;

  glm::vec3 front() const;
  glm::vec3 right() const;
  glm::vec3 up() const;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value& );
};

} // namespace compos


void SetTranslationWorld(
  const glm::vec3& origin,
  const entt::registry&,
  compos::Transform&,
  const compos::SceneNode& );

void SetOrientationWorld(
  const glm::quat& orientation,
  const entt::registry&,
  compos::Transform&,
  const compos::SceneNode& );

void SetScaleWorld(
  const glm::vec3& scale,
  const entt::registry&,
  compos::Transform&,
  const compos::SceneNode& );


glm::vec3 ToLocalSpace(
  const glm::vec3& point,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::quat ToLocalSpace(
  const glm::quat& orientation,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::mat4 ToLocalSpace(
  const glm::mat4& matrix,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::vec3 ToWorldSpace(
  const glm::vec3& point,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::quat ToWorldSpace(
  const glm::quat& orientation,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::mat4 ToWorldSpace(
  const glm::mat4& matrix,
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

glm::mat4 GetWorldMatrix(
  const entt::registry&,
  const compos::Transform&,
  const compos::SceneNode& );

} // namespace cqde
