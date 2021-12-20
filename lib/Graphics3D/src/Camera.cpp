#include <Graphics3D/Camera.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace Graphics3D
{

Camera::Camera(
  const Projection& projection,
  const float fov,
  const glm::vec4& viewport,
  const std::pair <float, float>& zRange )
  : mFov(fov)
  , mViewport(viewport)
  , mProjection(projection)
  , mZrange(zRange)
{}

void
Camera::setFov( const float radians )
{
  mFov = radians;
}

void
Camera::setViewport( const glm::vec4& viewport )
{
  mViewport = viewport;
}

void
Camera::setProjection( const Projection& projection )
{
  mProjection = projection;
}

void
Camera::setZrange( const std::pair <float, float>& zRange )
{
  mZrange = zRange;
}

glm::vec3
Camera::front() const
{
  return glm::rotate( mOrientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Camera::right() const
{
  return glm::rotate( mOrientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Camera::up() const
{
  return glm::rotate( mOrientation, {0.0f, 1.0f, 0.0f} );
}

glm::mat4
Camera::viewMatrix() const
{
  return glm::inverse( modelWorld() );
}

glm::mat4
Camera::projMatrix() const
{
  return  mProjection == Projection::Perspective ?
          glm::perspectiveFovRH_ZO( mFov,
                                    mViewport.z, mViewport.w,
                                    mZrange.first, mZrange.second )
        : glm::ortho( mViewport.x, mViewport.z, mViewport.y, mViewport.w,
                      mZrange.first, mZrange.second );
}

glm::vec4
Camera::viewport() const
{
  return mViewport;
}

float
Camera::fov() const
{
  return mFov;
}

Camera::Projection
Camera::projection() const
{
  return mProjection;
}

std::pair <float, float>
Camera::zRange() const
{
  return mZrange;
}

} // namespace Graphics3D
