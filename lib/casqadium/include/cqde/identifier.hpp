#pragma once

#include <cqde/fwd.hpp>

#include <entt/core/hashed_string.hpp>

#include <string>


namespace cqde
{

class identifier
{
  std::string mStr {"null"};
  entt::hashed_string mHash {"null"};

public:
  identifier() = default;

  identifier( const char* );
  identifier( const std::string& );
  identifier( const entt::hashed_string& );

  void setId( const std::string& );

  std::string str() const;
  entt::hashed_string hash() const;

  bool operator == ( const identifier& ) const;
  bool operator != ( const identifier& ) const;

  bool operator < ( const identifier& ) const;
};

struct identifier_hash
{
  std::size_t operator () ( const identifier& id ) const;
  std::size_t operator () ( const types::EntityReference& ) const;
};


const identifier null_id {"null"};

} // namespace cqde

cqde::identifier operator ""_id ( const char*,
                                  const size_t );
