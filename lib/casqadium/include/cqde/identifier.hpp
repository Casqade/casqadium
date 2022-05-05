#pragma once

#include <entt/core/hashed_string.hpp>

#include <string>


namespace cqde
{

class identifier
{
  std::string mStr;
  entt::hashed_string mHash;

public:
  identifier( const char* );
  identifier( const std::string& = "null" );
  identifier( const entt::hashed_string& );

  void setId( const std::string& );

  std::string str() const;
  entt::hashed_string hash() const;

  bool operator == ( const identifier& ) const;
  bool operator < ( const identifier& ) const;
};

const identifier null_id {"null"};

} // namespace cqde
