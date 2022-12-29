#pragma once

#include <cqde/conversion/json_glm_vec.hpp>

#include <glm/vec3.hpp>

#include <json/forwards.h>


namespace cqde
{

extern template
void operator << ( Json::Value&, const glm::bvec3& );

extern template
void operator << ( glm::bvec3&, const Json::Value& );

} // namespace cqde
