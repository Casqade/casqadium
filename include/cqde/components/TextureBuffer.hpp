#pragma once

#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <glm/vec2.hpp>

#include <json/forwards.h>

#include <vector>


namespace cqde::compos
{

struct alignas(64) TextureBuffer
{
  TextureId texture {};
  std::vector <glm::vec2> uvBuffer
  {
    {0.0f, 1.0f},
    {1.0f, 1.0f},
    {1.0f, 0.0f},
    {0.0f, 0.0f},
  };

  uint32_t uv {};

  mutable uint32_t refCounter {};


  TextureBuffer();
  TextureBuffer( const TextureBuffer& );
  TextureBuffer( TextureBuffer&& );
  ~TextureBuffer();

  TextureBuffer& operator = ( const TextureBuffer& );
  TextureBuffer& operator = ( TextureBuffer&& );

  void create();
  void update();
  void destroy();


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

} // namespace cqde::compos
