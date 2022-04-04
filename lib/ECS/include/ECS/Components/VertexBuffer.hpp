#ifndef ECS_COMPONENTS_VERTEXBUFFER_HPP
#define ECS_COMPONENTS_VERTEXBUFFER_HPP

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
  enum class PrimitiveType
  {
    Quad,
    Triangle,
  } primitiveType;


  std::vector <glm::vec3> vertexes;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

struct VertexBuffer
{
  std::vector <olc::vf2d> vertexes;

  float depth;

  enum class WindingOrder
  {
    CounterClockWise,
    ClockWise,

  } windingOrder;

  bool operator > ( const VertexBuffer& other ) const;
};

} // namespace Components

} // namespace ECS

#endif
