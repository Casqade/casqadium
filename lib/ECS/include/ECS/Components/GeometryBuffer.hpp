#ifndef ECS_COMPONENTS_GEOMETRY_BUFFER_HPP
#define ECS_COMPONENTS_GEOMETRY_BUFFER_HPP

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <vector>
#include <string>


namespace ECS
{

namespace Components
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


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
