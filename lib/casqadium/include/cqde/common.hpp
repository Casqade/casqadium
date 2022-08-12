#pragma once

#include <entt/fwd.hpp>


// assert in debug build
// handle condition safely in release
#define CQDE_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}

namespace cqde
{

void engineInit( entt::registry& registry );

std::string toLowerCase( const std::string& );

std::vector <std::string> stringSplit(  const std::string&,
                                        const char delimeter );

std::string stringJoin( const std::vector <std::string>&,
                        const std::string& delimeter = {} );

} // namespace cqde
