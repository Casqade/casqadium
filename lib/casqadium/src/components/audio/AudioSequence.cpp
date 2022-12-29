#include <cqde/components/audio/AudioSequence.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


namespace cqde::compos
{

AudioSequence::AudioSequence()
  : queue{std::make_unique <SoloudQueue> ()}
{}

AudioSequence::AudioSequence(
  AudioSequence&& other )
{
  *this = std::move(other);
}

AudioSequence&
AudioSequence::operator = (
  AudioSequence&& other )
{
  queue = std::move(other.queue);
  other.queue.reset();

  return *this;
}

AudioSequence::~AudioSequence()
{
  queue.reset();
}

Json::Value
AudioSequence::serialize() const
{
  return Json::ValueType::objectValue;
}

void
AudioSequence::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value&,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <AudioSequence> (entity);
}

} // namespace cqde::compos
