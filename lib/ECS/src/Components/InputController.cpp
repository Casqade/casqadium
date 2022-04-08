#include <ECS/Components/InputController.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

void
InputController::serialize( const std::string& json ) const
{}

void
InputController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const std::string& content ) const
{
  auto& comp = registry.emplace <InputController> (entity);
}

void
InputController::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <InputController> ();
  factory.type(entt::hashed_string{"InputController"});
  factory.data <&InputController::inputs> ("inputs"_hs);
  factory.func <&InputController::serialize> ("serialze"_hs);
  factory.func <&InputController::deserialize> ("deserialze"_hs);
}


} // namespace Components

} // namespace ECS
