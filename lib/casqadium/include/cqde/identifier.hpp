#pragma once

#include <cqde/fwd.hpp>

#include <entt/core/hashed_string.hpp>

#include <string>


namespace cqde
{

class identifier
{
  using hash_type = entt::hashed_string::hash_type;

  std::string mStr {"null"};
  hash_type mHash {entt::hashed_string{"null"}};

public:
  identifier() = default;

  identifier( const char* );
  identifier( const std::string& );

  void setId( const std::string& );

  std::string str() const;
  hash_type hash() const;

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

namespace literals
{

identifier operator ""_id ( const char*, const size_t );

} // namespace literals

} // namespace cqde
