#pragma once

#include <cqde/conversion/json_glm_vec.hpp>

#include <glm/gtx/quaternion.hpp>

#include <json/forwards.h>


namespace cqde
{

extern template
void operator << ( Json::Value&, const glm::quat& );

extern template
void operator << ( glm::quat&, const Json::Value& );

} // namespace cqde
