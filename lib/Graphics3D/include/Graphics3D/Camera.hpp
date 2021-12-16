#ifndef GRAPHICS_3D_CAMERA_HPP
#define GRAPHICS_3D_CAMERA_HPP

#include <Graphics3D/Transformable.hpp>


namespace Graphics3D
{

class Camera : public Transformable
{
public:
  enum class Projection : bool
  {
    Perspective,
    Orthogonal
  };

private:
  float mFov;
  glm::vec4 mViewport;
  Projection mProjection;
  std::pair <float, float> mZrange;

  void setScale( const glm::vec3& ) = delete;
  void scale( const glm::vec3& ) = delete;
  glm::vec3 scale() const = delete;

public:
  Camera( const Projection& = Projection::Perspective,
          const float fov = 45.0f,
          const glm::vec4& viewport = {0.0f, 0.0f, 800.0f, 600.0f},
          const std::pair <float, float>& zRange = {0.1f, 1000.0f} );

  void setFov( const float degrees );
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

} // namespace Graphics3D


#endif
