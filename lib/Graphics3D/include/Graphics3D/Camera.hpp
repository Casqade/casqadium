#ifndef GRAPHICS_3D_CAMERA_HPP
#define GRAPHICS_3D_CAMERA_HPP

#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

class Camera : public SceneNode
{
public:
  enum class Projection : bool
  {
    Perspective,
    Orthographic
  };

private:
  float mFov;
  glm::vec4 mViewport;
  Projection mProjection;
  std::pair <float, float> mZrange;

public:
  Camera( const Projection& = Projection::Perspective,
          const float fov = glm::radians(45.0f),
          const glm::vec4& viewport = {0.0f, 0.0f, 800.0f, 600.0f},
          const std::pair <float, float>& zRange = {0.1f, 1000.0f} );

  void setFov( const float radians );
  void setViewport( const glm::vec4& );
  void setProjection( const Projection& );
  void setZrange( const std::pair <float, float>& );

  glm::vec3 front() const;
  glm::vec3 right() const;
  glm::vec3 up() const;

  glm::mat4 viewMatrix() const;
  glm::mat4 projMatrix() const;
  glm::vec4 viewport() const;

  float fov() const;
  Projection projection() const;
  std::pair <float, float> zRange() const;
};

class CameraFPS : public Camera
{
  enum ViewMatrixVector
  {
    Right,
    Up,
    Front
  };

public:
  void rotate( const glm::quat& ) override;
  void rotateGlobal( const glm::quat& ) override;
};

} // namespace Graphics3D


#endif
