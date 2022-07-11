#include <cqde/conversion/json_glm_vec1.hpp>
#include <cqde/conversion/json_glm_vec2.hpp>
#include <cqde/conversion/json_glm_vec3.hpp>
#include <cqde/conversion/json_glm_vec4.hpp>
#include <cqde/conversion/json_glm_quat.hpp>

#include <spdlog/fmt/bundled/format.h>


namespace cqde
{

template <typename Vec,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::x)>, bool>::type>
void
operator << (
  Json::Value& json,
  const Vec& vec )
{
  for ( glm::length_t i = 0;
        i < vec.length();
        ++i )
    json.append(vec[i]);
}

template <typename Vec,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::x)>, bool>::type>
void
operator << (
  Vec& vec,
  const Json::Value& json )
{
  using fmt::format;

  if ( json.isArray() == false )
    throw std::runtime_error("json must be a JSON array");

  if ( json.size() != vec.length() )
    throw std::runtime_error(
      format("array must contain {} elements",
             vec.length()));

  if ( json[0].isDouble() == false )
    throw std::runtime_error("array element must be a JSON float");

  for ( glm::length_t i = 0;
        i < vec.length();
        ++i )
    vec[i] = json[i].asFloat();
}

template
void operator << ( Json::Value&, const glm::vec1& );

template
void operator << ( Json::Value&, const glm::vec2& );

template
void operator << ( Json::Value&, const glm::vec3& );

template
void operator << ( Json::Value&, const glm::vec4& );

template
void operator << ( Json::Value&, const glm::quat& );

template
void operator << ( glm::vec1&, const Json::Value& );

template
void operator << ( glm::vec2&, const Json::Value& );

template
void operator << ( glm::vec3&, const Json::Value& );

template
void operator << ( glm::vec4&, const Json::Value& );

template
void operator << ( glm::quat&, const Json::Value& );

} // namespace cqde
