#include <cqde/conversion/rp3d_glm.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <reactphysics3d/mathematics/Vector2.h>
#include <reactphysics3d/mathematics/Vector3.h>
#include <reactphysics3d/mathematics/Matrix2x2.h>
#include <reactphysics3d/mathematics/Matrix3x3.h>
#include <reactphysics3d/mathematics/Quaternion.h>
#include <reactphysics3d/mathematics/Transform.h>


namespace cqde
{

glm::vec2
rp3dToGlm(
  const rp3d::Vector2& vec )
{
  return
  {
    vec.x,
    vec.y,
  };
}

glm::vec3
rp3dToGlm(
  const rp3d::Vector3& vec )
{
  return
  {
    vec.x,
    vec.y,
    vec.z,
  };
}

glm::quat
rp3dToGlm(
  const rp3d::Quaternion& quat )
{
  return
  {
    quat.w,
    quat.x,
    quat.y,
    quat.z,
  };
}

glm::mat2
rp3dToGlm(
  const rp3d::Matrix2x2& mat )
{
  return
  {
    rp3dToGlm(mat.getColumn(0)),
    rp3dToGlm(mat.getColumn(1)),
  };
}

glm::mat3
rp3dToGlm(
  const rp3d::Matrix3x3& mat )
{
  return
  {
    rp3dToGlm(mat.getColumn(0)),
    rp3dToGlm(mat.getColumn(1)),
    rp3dToGlm(mat.getColumn(2)),
  };
}

glm::mat4
rp3dToGlm(
  const rp3d::Transform& transform )
{
  glm::mat4 mat {};
  transform.getOpenGLMatrix(glm::value_ptr(mat));

  return mat;
}


rp3d::Vector2
glmToRp3d(
  const glm::vec2& vec )
{
  return
  {
    vec.x,
    vec.y,
  };
}

rp3d::Vector3
glmToRp3d(
  const glm::vec3& vec )
{
  return
  {
    vec.x,
    vec.y,
    vec.z,
  };
}

rp3d::Quaternion
glmToRp3d(
  const glm::quat& quat )
{
  return
  {
    quat.x,
    quat.y,
    quat.z,
    quat.w,
  };
}

rp3d::Matrix2x2
glmToRp3d(
  const glm::mat2& mat )
{
  return
  {
    mat[0][0],
    mat[1][0],
    mat[0][1],
    mat[1][1],
  };
}

rp3d::Matrix3x3
glmToRp3d(
  const glm::mat3& mat )
{
  return
  {
    mat[0][0],
    mat[1][0],
    mat[2][0],
    mat[0][1],
    mat[1][1],
    mat[2][1],
    mat[0][2],
    mat[1][2],
    mat[2][2],
  };
}

rp3d::Transform
glmToRp3d(
  glm::mat4 mat )
{
  rp3d::Transform transform {};
  transform.setFromOpenGL(glm::value_ptr(mat));

  return transform;
}

} // namespace cqde
