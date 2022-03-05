#include <Graphics3D/Camera.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>


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
  , mRenderMode(RenderMode::Solid)
  , mTextureMode(TextureMode::Textured)
  , mLightingMode(LightingMode::Diffuse)
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

Camera::RenderMode
Camera::renderMode() const
{
  return mRenderMode;
}

Camera::TextureMode
Camera::textureMode() const
{
  return mTextureMode;
}

Camera::LightingMode
Camera::lightingMode() const
{
  return mLightingMode;
}


void
CameraController::control( const glm::vec3& angle )
{
  rotate( glm::angleAxis(angle.x, glm::vec3{1.0f, 0.0f, 0.0f}) );
  rotate( glm::angleAxis(angle.y, glm::vec3{0.0f, 1.0f, 0.0f}) );
  rotate( glm::angleAxis(angle.z, glm::vec3{0.0f, 0.0f, 1.0f}) );
}


CameraControllerFPS::CameraControllerFPS( const std::pair <glm::vec2, glm::vec2>& viewLimits )
  : CameraController()
  , mViewCurrent()
  , mViewLimits(viewLimits)
  , mUpAxis(up())
{}


void
CameraControllerFPS::clampViewAxis( const std::pair <float, float> valueLimits,
                                    float& value )
{
  if (    valueLimits.first == -std::numeric_limits <float>::infinity()
       && value < glm::radians(-180.0f) )
    value += glm::radians(360.0f);

  if (    valueLimits.second == std::numeric_limits <float>::infinity()
       && value > glm::radians(180.0f) )
    value -= glm::radians(360.0f);
}

void
CameraControllerFPS::control( const glm::vec3& angle )
{
  const auto viewLast = mViewCurrent;

  mViewCurrent = glm::clamp( mViewCurrent + glm::vec2(angle),
                             mViewLimits.first,
                             mViewLimits.second );

  clampViewAxis( {mViewLimits.first.x, mViewLimits.second.x},
                  mViewCurrent.x );

  clampViewAxis( {mViewLimits.first.y, mViewLimits.second.y},
                  mViewCurrent.y );

  rotate( glm::angleAxis( (mViewCurrent - viewLast).x, glm::vec3{1.0f, 0.0f, 0.0f} ) );
  rotateGlobal( glm::angleAxis( (mViewCurrent - viewLast).y, mUpAxis ) );
}

void
CameraControllerFPS::setOrientation( const glm::quat& orientation )
{
  Transformable::setOrientation( orientation );
  mViewCurrent = {};
  mUpAxis = up();
}

void
CameraControllerFPS::setUpAxis( const glm::vec3& axisUp )
{
  mUpAxis = axisUp;
}

void
CameraControllerFPS::setViewCurrent( const glm::vec2& viewCurrent )
{
  mViewCurrent = viewCurrent;
}

void
CameraControllerFPS::setViewLimits( const std::pair <glm::vec3, glm::vec3>& viewLimits )
{
  mViewLimits = viewLimits;
}

} // namespace Graphics3D
