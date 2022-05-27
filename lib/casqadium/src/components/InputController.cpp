#include <cqde/components/InputController.hpp>

#include <entt/entt.hpp>

#include <json/value.h>


namespace cqde::compos
{

void
InputController::serialize( Json::Value& json ) const
{}

void
InputController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& content ) const
{
  auto& comp = registry.emplace <InputController> (entity);
}

void
InputController::Register()
{
  using namespace entt::literals;

  auto factory = entt::meta <InputController> ();
  factory.type("InputController"_hs);
  factory.func <&InputController::serialize> ("serialize"_hs);
  factory.func <&InputController::deserialize> ("deserialize"_hs);
}


} // namespace cqde::compos
