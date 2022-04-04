#ifndef ECS_COMPONENTS_CAMERA_HPP
#define ECS_COMPONENTS_CAMERA_HPP

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>

#include <string>

#include <ECS/Components/Tag.hpp>
#include <ECS/Components/VertexBuffer.hpp>


namespace ECS
{

namespace Components
{

struct SceneNode;
struct Transform;


struct Camera
{
  enum class Projection : bool
  {
    Perspective,
    Orthographic,
  };

  enum ViewMatrixVector
  {
    Right,
    Up,
    Front,
  };

  enum class RenderMode
  {
    Wireframe,
    Solid,
  };

  enum class TextureMode
  {
    NoTexture,
    Color,
    Textured,
  };

  enum class LightingMode
  {
    Ambient,
    Diffuse,
  };

  float fov;
  glm::vec4 viewport;
  std::pair <float, float> zRange;

  Projection    projectionType;
  RenderMode    renderMode;
  TextureMode   textureMode;
  LightingMode  lightingMode;

  std::multimap < VertexBuffer,
                  entt::entity,
                  std::greater <VertexBuffer>> zBuffer;


  Camera( const Projection& = Projection::Perspective,
          const float fov = glm::radians(45.0f),
          const glm::vec4& viewport = {0.0f, 0.0f, 800.0f, 600.0f},
          const std::pair <float, float>& zRange = {0.1f, 1000.0f} );

  glm::mat4 projMatrix() const;
  glm::mat4 viewMatrix( entt::registry&   registry,
                        const SceneNode&  cSceneNode,
                        const Transform&  cTransform ) const;


  void serialize( const std::string& json ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const std::string& json ) const;

  static void Register();
};

} // namespace Components

} // namespace ECS

#endif
