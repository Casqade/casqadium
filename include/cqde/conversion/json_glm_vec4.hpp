#pragma once

#include <cqde/conversion/json_glm_vec.hpp>

#include <glm/vec4.hpp>

#include <json/forwards.h>


namespace cqde
{

extern template
void operator << ( Json::Value&, const glm::vec4& );

extern template
void operator << ( glm::vec4&, const Json::Value& );

} // namespace cqde
