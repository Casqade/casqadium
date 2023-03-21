#include <cqde/identifier.hpp>

#include <cqde/types/EntityReference.hpp>

#include <entt/core/hashed_string.hpp>


namespace cqde
{

identifier::identifier(
  const char* str )
  : mStr(str)
  , mHash(entt::hashed_string{str})
{}

identifier::identifier(
  const std::string& str )
  : mStr(str)
  , mHash(entt::hashed_string{str.c_str()})
{}

void
identifier::setId(
  const std::string& str )
{
  mStr = str;
  mHash = entt::hashed_string{mStr.c_str()};
}

std::string
identifier::str() const
{
  return mStr;
}

identifier::hash_type
identifier::hash() const
{
  return mHash;
}

bool
identifier::operator == (
  const identifier& other ) const
{
  return this->mHash == other.mHash;
}

bool
identifier::operator != (
  const identifier& other ) const
{
  return !(*this == other);
}

bool
identifier::operator < (
  const identifier& other ) const
{
  return this->mHash < other.mHash;
}

bool
identifier::operator > (
  const identifier& other ) const
{
  return !(*this < other);
}

std::size_t
identifier_hash::operator () (
  const identifier& id ) const
{
  return std::hash <entt::hashed_string::hash_type> ()(id.hash());
}

std::size_t
identifier_hash::operator () (
  const types::EntityReference& reference ) const
{
  return std::hash <entt::hashed_string::hash_type> ()(reference.id.hash());
}


namespace literals
{

identifier
operator ""_id (
  const char* str,
  const size_t size )
{
  return std::string{str, size};
}

} // namespace literals

} // namespace cqde
