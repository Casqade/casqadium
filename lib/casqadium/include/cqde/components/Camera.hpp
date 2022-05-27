#pragma once

#include <cqde/types/VertexBuffer.hpp>
#include <cqde/fwd.hpp>

#include <entt/fwd.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>

#include <json/forwards.h>


namespace cqde::compos
{

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

  glm::vec4 viewport {};
  std::pair <float, float> zRange = {0.1f, 1000.0f};
  float fov = glm::radians(45.0f);

  Projection    projectionType = Projection::Perspective;
  RenderMode    renderMode = RenderMode::Solid;
  TextureMode   textureMode = TextureMode::Textured;
  LightingMode  lightingMode = LightingMode::Diffuse;

  std::multimap < types::VertexBuffer, entt::entity,
                  std::greater <types::VertexBuffer>> zBuffer {};


  Camera() = default;
  Camera( const glm::vec4& viewport,
          const Projection projection = Projection::Perspective );

  glm::mat4 projMatrix() const;
  glm::mat4 viewMatrix( entt::registry&   registry,
                        const SceneNode&  cSceneNode,
                        const Transform&  cTransform ) const;


  void serialize( Json::Value& ) const;

  void deserialize( entt::registry&,
                    entt::entity,
                    const Json::Value& ) const;

  static void Register();
};

} // namespace cqde::compos
