#pragma once

#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

class Camera : public SceneNode
{
public:
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

private:
  float mFov;
  glm::vec4 mViewport;
  Projection mProjection;
  std::pair <float, float> mZrange;

  RenderMode    mRenderMode;
  TextureMode   mTextureMode;
  LightingMode  mLightingMode;

public:
  Camera( const Projection& = Projection::Perspective,
          const float fov = glm::radians(45.0f),
          const glm::vec4& viewport = {0.0f, 0.0f, 800.0f, 600.0f},
          const std::pair <float, float>& zRange = {0.1f, 1000.0f} );

  void setFov( const float radians );
  void setViewport( const glm::vec4& );
  void setProjection( const Projection& );
  void setZrange( const std::pair <float, float>& );

  glm::vec3 front() const override;

  glm::mat4 viewMatrix() const;
  glm::mat4 projMatrix() const;
  glm::vec4 viewport() const;

  float fov() const;
  Projection projection() const;
  std::pair <float, float> zRange() const;

  RenderMode renderMode() const;
  TextureMode textureMode() const;
  LightingMode lightingMode() const;
};

class CameraController : public SceneNode
{
public:
  CameraController() = default;
  virtual ~CameraController() = default;

  virtual void control( const glm::vec3& angleDelta );
};

class CameraControllerFPS : public CameraController
{
  glm::vec2 mViewCurrent;
  std::pair <glm::vec2, glm::vec2> mViewLimits;

  glm::vec3 mUpAxis;

  void clampViewAxis( const std::pair <float, float> valueLimits,
                      float& value );

public:
  CameraControllerFPS( const std::pair <glm::vec2,
                                        glm::vec2>& viewLimits = {glm::vec2{  glm::radians(-90.0f),
                                                                              -std::numeric_limits <float>::infinity()},
                                                                  glm::vec2{  glm::radians(90.0f),
                                                                              std::numeric_limits <float>::infinity()}});

  void control( const glm::vec3& angleDelta ) override;

  void setOrientation( const glm::quat& ) override;

  void setUpAxis( const glm::vec3& );
  void setViewCurrent( const glm::vec2& );
  void setViewLimits( const std::pair <glm::vec3, glm::vec3>& );
};

} // namespace Graphics3D
