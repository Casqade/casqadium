#pragma once

#include <type_traits>

#include <json/value.h>


namespace cqde
{

template <class Vec,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::x)>, bool>::type = true>
void
operator << ( Json::Value&,const Vec& );

template <typename Vec,
typename std::enable_if <std::is_arithmetic_v <decltype(Vec::x)>, bool>::type = true>
void operator << ( Vec&, const Json::Value& );

} // namespace cqde
