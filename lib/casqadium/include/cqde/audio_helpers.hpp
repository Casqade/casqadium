#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>


namespace cqde
{

struct AudioListenerParameters
{
  glm::mat4 transform {1.0f};
  glm::vec3 velocity {};
};

struct AudioSourceParams
{
  glm::vec3 pos {};
  glm::vec3 velocity {};
};

AudioListenerParameters
calcAudioListenerParams(
  entt::registry&,
  const entt::entity eListener,
  const compos::Transform& cListenerTransform );

void
updateAudio3dParams(
  entt::registry&,
  const AudioHandle,
  const AudioListenerParameters&,
  const entt::entity eListener,
  const compos::Transform& cListenerTransform,
  const entt::entity eSource,
  const compos::Transform& cSourceTransform );

} // namespace cqde
