#include <GameStateEcsSandbox.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <ECS/Common.hpp>
#include <ECS/Components/Transform.hpp>


GameStateEcsSandbox::GameStateEcsSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
{
  using namespace entt::literals;

  entt::registry reg;
  ECS::registryInit(reg);

  auto entity = reg.create();

  auto resolved = entt::resolve("Transform"_hs);
  auto any = resolved.construct();
  auto invoked = any.invoke("deserialze"_hs, std::ref(reg), entity, std::string("newcontent"));

  std::cout << bool(resolved) << bool(any) << bool(invoked) << "\n";

  for(const auto &&[ent, comp] : reg.view <ECS::Components::Transform>().each())
  {
    std::cout << comp.translation.x << "\n";
  }
  std::cout << "size " << reg.size() << reg.size <ECS::Components::Transform> () << "\n";
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  return false;
}
