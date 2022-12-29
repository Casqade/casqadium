#pragma once

#include <cqde/conversion/json_glm_vec.hpp>

#include <glm/gtc/vec1.hpp>

#include <json/forwards.h>


namespace cqde
{

extern template
void operator << ( Json::Value&, const glm::vec1& );

extern template
void operator << ( glm::vec1&, const Json::Value& );

} // namespace cqde
