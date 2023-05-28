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

struct CarEngine
{
  struct
  {
    float current {};

    float max {1000.0f};

  } torque {};

  struct
  {
    float current {};

    float idle {600.0f};
    float optimal {2500.0f};
    float redline {6000.0f};
    float max {7000.0f};
    float sensitivity {2.0f};

  } rpm {};

  struct
  {
    float ambient {20.0f};
    float optimal {70.0f};
    float max {110.0f};

    float current {ambient};

    float sensitivity {2.5f};
    float coolingRate {0.05f};

  } temperature {};

  struct
  {
    float current {};

    float base {0.2f};

  } soundVolume {};

  struct
  {
    float current {};

    float min {0.5f};
    float max {1.4f};

  } soundSpeed {};

  struct
  {
    float current {};

    float lower {200.0f};
    float upper {20'000.0f};

  } cutoffFrequency {};


  float torqueCurve() const;
  float temperatureFactor() const;


  CarEngine() = default;

  void update(
    const float throttle,
    const float clutch,
    const float dt );

  void ui_show();

  Json::Value serialize() const;
  void deserialize( const Json::Value& );
};

struct EngineController
{
  using EntityId = cqde::EntityId;
  using AudioId = cqde::AudioId;
  using identifier_hash = cqde::identifier_hash;

  AudioId cylinderAudioId {};

  CarEngine engine {};

  float throttle {};
  float clutch {};


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
