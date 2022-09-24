#pragma once

#include <cqde/types/VertexBuffer.hpp>
#include <cqde/alias.hpp>

#include <entt/fwd.hpp>

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>

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
    Solid,
    Wireframe,
  };

  enum class TextureMode
  {
    Textured,
    NoTexture,
  };

  enum class LightingMode
  {
    Diffuse,
    FullBright,
  };

  std::multimap < types::VertexBuffer, entt::entity,
                  types::VertexBufferComparator> zBuffer {};

  glm::vec4 viewport {0.0, 0.0, 1.0, 1.0};
  std::pair <float, float> zRange {0.1f, 1000.0f};

  union
  {
    float fov {1.0f}; // = 57 degrees
    float zoom;
  };

  int32_t layer {};

  Projection    projectionType = Projection::Perspective;
  RenderMode    renderMode = RenderMode::Solid;
  TextureMode   textureMode = TextureMode::Textured;
  LightingMode  lightingMode = LightingMode::Diffuse;


  Camera() = default;
  Camera( const glm::vec4& viewport,
          const Projection projection = Projection::Perspective );

  glm::vec4 viewportScaled() const;

  glm::mat4 projMatrix() const;
  glm::mat4 viewMatrix( entt::registry&   registry,
                        const SceneNode&  cSceneNode,
                        const Transform&  cTransform ) const;


  static std::string ProjectionTypeToString( const Projection );
  static std::string RenderModeToString( const RenderMode );
  static std::string TextureModeToString( const TextureMode );
  static std::string LightingModeToString( const LightingMode );

  static Projection ProjectionTypeFromString( const std::string& );
  static RenderMode RenderModeFromString( const std::string& );
  static TextureMode TextureModeFromString( const std::string& );
  static LightingMode LightingModeFromString( const std::string& );


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
