#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <json/forwards.h>


namespace demo
{

struct MusicalInstrument
{
  using AudioId = cqde::AudioId;
  using AudioHandle = cqde::AudioHandle;
  using EntityId = cqde::EntityId;
  using identifier_hash = cqde::identifier_hash;

  AudioId audioId {};
  AudioHandle handle {cqde::AudioHandleInvalid};


  MusicalInstrument() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

struct MusicConcertController
{
  using AudioHandle = cqde::AudioHandle;
  using EntityId = cqde::EntityId;
  using identifier_hash = cqde::identifier_hash;

  AudioHandle groupHandle {};


  MusicConcertController() = default;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

struct FootstepAudioBank
{
  using EntityId = cqde::EntityId;
  using identifier_hash = cqde::identifier_hash;

  FootstepAudioBank() = default;


  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

struct PhysicsMaterial
{
  using EntityId = cqde::EntityId;
  using identifier = cqde::identifier;
  using identifier_hash = cqde::identifier_hash;

  identifier materialId {};


  PhysicsMaterial() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

struct EngineController
{
  using EntityId = cqde::EntityId;
  using AudioId = cqde::AudioId;
  using identifier_hash = cqde::identifier_hash;

  AudioId cylinderAudioId {};


  EngineController() = default;


  void ui_edit_props( const entt::entity,
                      const entt::registry& );

  Json::Value serialize() const;

  static void deserialize(
    entt::registry&,
    entt::entity,
    const Json::Value&,
    const std::unordered_map <EntityId, EntityId,
                              identifier_hash>& idMap = {} );
};

} // namespace demo
