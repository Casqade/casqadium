#include <Graphics3D/Transformable.hpp>


namespace Graphics3D
{

Transformable::Transformable(
  const glm::vec3& origin,
  const glm::quat& orientation,
  const glm::vec3& scale )
  : mOrigin(origin)
  , mOrientation(orientation)
  , mScale(scale)
{}

glm::mat4
Transformable::modelLocal() const
{
  return  glm::translate( glm::mat4(1.0f), mOrigin )
        * glm::toMat4(mOrientation)
        * glm::scale( glm::mat4(1.0f), mScale );
}

void
Transformable::translate( const glm::vec3& translation )
{
  mOrigin += translation;
}

void
Transformable::rotate( const glm::quat& rotation )
{
  mOrientation = glm::normalize(mOrientation * rotation);
}

void
Transformable::rotateGlobal( const glm::quat& rotation )
{
  mOrientation = glm::normalize(rotation * mOrientation);
}

void
Transformable::scale( const glm::vec3& scale )
{
  mScale += scale;
}

void
Transformable::setOrigin( const glm::vec3& origin )
{
  mOrigin = origin;
}

void
Transformable::setOrientation( const glm::quat& orientation )
{
  mOrientation = orientation;
}

void
Transformable::setScale( const glm::vec3& scale )
{
  mScale = scale;
}

glm::vec3
Transformable::origin() const
{
  return mOrigin;
}

glm::quat
Transformable::orientation() const
{
  return mOrientation;
}

glm::vec3
Transformable::scale() const
{
  return mScale;
}

glm::vec3
Transformable::front() const
{
  return glm::rotate( mOrientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Transformable::right() const
{
  return glm::rotate( mOrientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Transformable::up() const
{
  return glm::rotate( mOrientation, {0.0f, 1.0f, 0.0f} );
}

} // namespace Graphics3D
