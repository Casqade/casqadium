#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>


namespace Graphics3D
{

class Transformable
{
protected:
  glm::vec3 mOrigin;
  glm::quat mOrientation;
  glm::vec3 mScale;

public:
  Transformable(  const glm::vec3& origin = {},
                  const glm::quat& orientation = glm::vec3{},
                  const glm::vec3& scale = { 1.0f, 1.0f, 1.0f } );
  Transformable( Transformable& ) = default;

  virtual ~Transformable() = default;

  virtual glm::mat4 modelLocal() const;

  virtual void translate( const glm::vec3& );
  virtual void rotate( const glm::quat& );
  virtual void rotateGlobal( const glm::quat& );
  virtual void scale( const glm::vec3& );

  virtual void setOrigin( const glm::vec3& );
  virtual void setOrientation( const glm::quat& );
  virtual void setScale( const glm::vec3& );

  virtual glm::vec3 origin() const;
  virtual glm::quat orientation() const;
  virtual glm::vec3 scale() const;

  virtual glm::vec3 front() const;
  virtual glm::vec3 right() const;
  virtual glm::vec3 up() const;
};

} // namespace Graphics3D
