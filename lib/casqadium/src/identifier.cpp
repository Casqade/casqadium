#include <cqde/identifier.hpp>

#include <entt/core/hashed_string.hpp>


namespace cqde
{

identifier::identifier( const char* str )
  : mStr(str)
  , mHash(mStr.data())
{}

identifier::identifier( const std::string& str )
  : mStr(str)
  , mHash(mStr.data())
{}

identifier::identifier( const entt::hashed_string& str )
  : mStr(str.data())
  , mHash(mStr.data())
{}

void
identifier::setId( const std::string& str )
{
  mStr = str;
  mHash = entt::hashed_string{mStr.data()};
}

std::string
identifier::str() const
{
  return mStr;
}

entt::hashed_string
identifier::hash() const
{
  return mHash;
}

bool
identifier::operator == ( const identifier& other ) const
{
  return this->mHash.value() == other.mHash.value();
}

bool
identifier::operator != ( const identifier& other ) const
{
  return !(*this == other);
}

bool
identifier::operator < ( const identifier& other ) const
{
  return this->mHash.value() < other.mHash.value();
}

} // namespace cqde

cqde::identifier
operator ""_id ( const char* str,
                 const size_t size )
{
  return std::string{str, size};
}
