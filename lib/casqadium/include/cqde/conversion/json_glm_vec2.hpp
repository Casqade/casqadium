#pragma once

#include <cqde/conversion/json_glm_vec.hpp>

#include <glm/vec2.hpp>

#include <json/forwards.h>


namespace cqde
{

extern template
void operator << ( Json::Value&, const glm::vec2& );

extern template
void operator << ( glm::vec2&, const Json::Value& );

} // namespace cqde
