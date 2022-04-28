#pragma once

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <json/forwards.h>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>


namespace cqde::compos
{

constexpr std::array <glm::vec3, 4> defaultQuadVerts =
{{
  {-0.5f, 0.5f, 0.0f},
  {-0.5f, -0.5f, 0.0f},
  {0.5f, -0.5f, 0.0f},
  {0.5f, 0.5f, 0.0f}
}};

struct GeometryBuffer
{
  std::vector <glm::vec3> vertexes;

  GeometryBuffer( const std::vector <glm::vec3>& );
  GeometryBuffer( const std::array <glm::vec3, 4>& = defaultQuadVerts );


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
