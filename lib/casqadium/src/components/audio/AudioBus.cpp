#include <cqde/components/audio/AudioBus.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

AudioBus::AudioBus()
  : bus{std::make_unique <SoLoud::Bus> ()}
{}

AudioBus::AudioBus(
  AudioBus&& other )
{
  *this = std::move(other);
}

AudioBus&
AudioBus::operator = (
  AudioBus&& other )
{
  bus = std::move(other.bus);
  other.bus.reset();

  return *this;
}

AudioBus::~AudioBus()
{
  bus.reset();
}

void
AudioBus::ensureIsPlaying(
  SoLoud::Soloud& soloud )
{
  const auto handle = bus->mChannelHandle;

  if ( soloud.isValidVoiceHandle(handle) == true )
    return;

  soloud.play(*bus);
  bus->findBusHandle();
}

Json::Value
AudioBus::serialize() const
{
  return Json::ValueType::objectValue;
}

void
AudioBus::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <AudioBus> (entity);
}

} // namespace cqde::compos
