#include <cqde/string_helpers.hpp>

#include <locale>
#include <codecvt>
#include <sstream>


namespace cqde
{

std::string
toLowerCase(
  const std::string& str )
{
  std::string result = str;

  for ( auto& c : result )
    c = std::tolower(c);

  return result;
}

std::vector <std::string>
stringSplit(
  const std::string& string,
  const char delimeter )
{
  std::vector <std::string> result {};

  std::stringstream stream {string};
  std::string token {};

  while ( getline (stream, token, delimeter) )
    result.push_back(token);

  return result;
}

std::string
stringJoin(
  const std::vector <std::string>& strings,
  const std::string& delimeter )
{
  std::string result {};

  for ( auto iter = strings.begin();
        iter != strings.end();
        ++iter )
    if ( iter == strings.end() - 1 )
      result += *iter;
    else
      result += *iter + delimeter;

  return result;
}

std::string
stringToUTF8(
  const std::u32string& source )
{
  return std::wstring_convert <std::codecvt_utf8_utf16 <char32_t>, char32_t> {}.to_bytes(source);
}

std::u32string
stringFromUTF8(
  const std::string& source )
{
  return std::wstring_convert <std::codecvt_utf8_utf16 <char32_t>, char32_t> {}.from_bytes(source);
}

} // namespace cqde
