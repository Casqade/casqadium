#include <cqde/callbacks/audio.hpp>

#include <cqde/components/audio/AudioBus.hpp>
#include <cqde/components/audio/AudioListener3d.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SubscriberUpdate.hpp>

#include <entt/entity/registry.hpp>


namespace cqde::callbacks
{

void
audioStopAll(
  entt::registry& registry,
  const std::vector <std::any>& )
{
  registry.ctx().get <SoLoud::Soloud> ().stopAll();
}

} // namespace cqde::callbacks
