#pragma once

#include <entt/fwd.hpp>

#include <cassert>


// assert in debug build
// handle condition safely in release
#define CQDE_ASSERT_DEBUG(cond, action)\
  if (!(cond)) {assert(cond); action;}

namespace cqde
{

void engineInit( entt::registry& registry );

std::string toLowerCase( const std::string& );

std::vector <std::string> stringSplit(
  const std::string&,
  const char delimeter );

std::string stringJoin(
  const std::vector <std::string>&,
  const std::string& delimeter = {} );

std::string stringToUTF8( const std::u32string& );
std::u32string stringFromUTF8( const std::string& );


template <class Container>
std::string
IdGenerate(
  const Container& container,
  const std::string& hint,
  const std::string& prefix,
  const char delimeter = '_' )
{
  uint64_t id {};

  auto tokens = stringSplit(hint, delimeter);

  if ( tokens.empty() == true )
    tokens.push_back(prefix);

  try
  {
    id = std::stoi(tokens.back());
  }
  catch ( ... )
  {
    tokens.push_back(std::to_string(id));
  }

  while ( container.count(stringJoin(tokens, {delimeter}) ) > 0 )
    tokens.back() = std::to_string(++id);

  return stringJoin(tokens, {delimeter});
}

} // namespace cqde
