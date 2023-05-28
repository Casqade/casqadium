#include <demo/systems.hpp>
#include <demo/components.hpp>

#include <cqde/render_helpers.hpp>

#include <cqde/components/SubscriberUpdate.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <cqde/types/TickCurrent.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/constants.hpp>

#include <reactphysics3d/body/RigidBody.h>

#include <spdlog/fmt/bundled/format.h>


namespace demo
{

void
EngineSystem(
  entt::registry& registry )
{
  using fmt::format;
  using cqde::compos::RigidBody;
  using cqde::compos::SubscriberUpdate;
  using cqde::types::TickCurrent;
  using cqde::types::TextureAssetManager;

  auto& textures = registry.ctx().get <TextureAssetManager> ();

  const auto& tick = registry.ctx().get <TickCurrent> ();
  const auto dt = static_cast <float> (tick.tickInterval);

  for ( auto&& [eEngine, cEngine, cRigidBody]
    : registry.view <EngineController, RigidBody, SubscriberUpdate> ().each() )
  {
    auto& engine = cEngine.engine;

    engine.update(cEngine.throttle, 1.0f, dt);
    cRigidBody.body->setAngularVelocity({-2.0f * glm::pi <float> () * (1.0f + engine.rpm.current / engine.rpm.max), 0.0f, 0.0f});

    const auto rpmTextColor = engine.rpm.current < engine.rpm.redline
      ? olc::WHITE
      : olc::RED;

    const auto temperatureTextColor = engine.temperature.current < engine.temperature.max
      ? olc::WHITE
      : olc::RED;

    const auto rpmText = format("{:4.0f}", engine.rpm.current);
    const auto torqueText = format("{:4.0f}", engine.torque.current);
    const auto temperatureText = format("{:3.1f}", engine.temperature.current);

    textures.insert( "engine_rpm",
      cqde::textureFromText(rpmText, rpmTextColor, olc::BLANK, true) );

    textures.insert( "engine_torque",
      cqde::textureFromText(torqueText, olc::WHITE, olc::BLANK, true) );

    textures.insert( "engine_temperature",
      cqde::textureFromText(temperatureText, temperatureTextColor, olc::BLANK, true) );
  }
}

} // namespace demo
