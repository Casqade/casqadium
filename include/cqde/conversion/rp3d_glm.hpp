#pragma once

#include <cqde/rp3d_fwd.hpp>

#include <glm/fwd.hpp>


namespace cqde
{

glm::vec2 rp3dToGlm( const rp3d::Vector2& );
glm::vec3 rp3dToGlm( const rp3d::Vector3& );
glm::quat rp3dToGlm( const rp3d::Quaternion& );

glm::mat2 rp3dToGlm( const rp3d::Matrix2x2& );
glm::mat3 rp3dToGlm( const rp3d::Matrix3x3& );

glm::mat4 rp3dToGlm( const rp3d::Transform& );


rp3d::Vector2 glmToRp3d( const glm::vec2& );
rp3d::Vector3 glmToRp3d( const glm::vec3& );
rp3d::Quaternion glmToRp3d( const glm::quat& );

rp3d::Matrix2x2 glmToRp3d( const glm::mat2& );
rp3d::Matrix3x3 glmToRp3d( const glm::mat3& );

rp3d::Transform glmToRp3d( glm::mat4 );

} // namespace cqde
