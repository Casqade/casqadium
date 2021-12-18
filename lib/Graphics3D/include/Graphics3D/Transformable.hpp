#ifndef GRAPHICS_3D_TRANSFORMABLE_HPP
#define GRAPHICS_3D_TRANSFORMABLE_HPP

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

  virtual glm::mat4 model() const;

  void translate( const glm::vec3& );
  void rotate( const glm::quat& );
  void rotateGlobal( const glm::quat& );
  void scale( const glm::vec3& );

  void setOrigin( const glm::vec3& );
  void setOrientation( const glm::quat& );
  void setScale( const glm::vec3& );

  glm::vec3 origin() const;
  glm::quat orientation() const;
  glm::vec3 scale() const;
};

} // namespace Graphics3D


#endif
