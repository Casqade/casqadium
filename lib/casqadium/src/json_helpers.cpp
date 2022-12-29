#include <cqde/json_helpers.hpp>

#include <cqde/common.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>


namespace cqde
{

std::string
jsonToString(
  const Json::Value& json,
  const bool withComments )
{
  if ( withComments == true)
    return Json::writeString(jsonWriter(), json);

  Json::StreamWriterBuilder writer = jsonWriter();

  writer["commentStyle"] = "None";

  return Json::writeString(writer, json);
}

Json::CharReaderBuilder
jsonReader()
{
  static Json::CharReaderBuilder reader =
  []
  {
    Json::CharReaderBuilder reader {};

    reader["collectComments"] = true;
    reader["allowComments"] = true;
    reader["allowTrailingCommas"] = true;
    reader["strictRoot"] = true;
    reader["allowDroppedNullPlaceholders"] = false;
    reader["allowNumericKeys"] = false;
    reader["allowSingleQuotes"] = false;
    reader["stackLimit"] = 1000;
    reader["failIfExtra"] = false;
    reader["rejectDupKeys"] = true;
    reader["allowSpecialFloats"] = true;
    reader["skipBom"] = true;

    return reader;
  }();

  return reader;
}

Json::StreamWriterBuilder
jsonWriter()
{
  static Json::StreamWriterBuilder writer =
  []
  {
    Json::StreamWriterBuilder writer {};

    writer["indentation"] = "  ";
    writer["commentStyle"] = "All";
    writer["precisionType"] = "  ";
    writer["enableYAMLCompatibility"] = false;
    writer["dropNullPlaceholders"] = false;
    writer["useSpecialFloats"] = true;
    writer["emitUTF8"] = true;
    writer["precision"] = 17;
    writer["precisionType"] = "significant";

    return writer;
  }();

  return writer;
}

void
jsonValidateArray(
  const Json::Value& array,
  const Json::Value& reference )
{
  using fmt::format;

  if ( array.type() != reference.type() )
    jsonThrowValidationError(reference);

  if ( reference.size() == 0 )
    return;

  for ( Json::ArrayIndex i = 0;
        i < array.size();
        ++i )
  {
    if ( jsonValidateValue(array[i], *reference.begin()) == false )
      jsonThrowValidationError(*reference.begin(), i);

    else if ( array[i].isObject() == true )
      jsonValidateObject( array[i], *reference.begin() );

    else if ( array[i].isArray() == true )
      jsonValidateArray( array[i], *reference.begin() );
  }
}

void
jsonValidateObject(
  const Json::Value& value,
  const Json::Value& reference )
{
  using fmt::format;

  if ( value.type() != reference.type() )
    jsonThrowValidationError(reference);

  if ( reference.isMember("cqde_json_anykey") == true )
  {
    const auto valueReference = reference["cqde_json_anykey"];

    for ( const auto& key : value.getMemberNames() )
    {
      if ( jsonValidateValue(value[key], valueReference) == false )
      {
        try
        {
          jsonThrowValidationError(valueReference);
        }
        catch ( const std::exception& e )
        {
          throw std::runtime_error(format(
            "failed to parse '{}' value: {}",
            key, e.what() ));
        }
      }

      else if ( value[key].isObject() == true )
        jsonValidateObject(value[key], valueReference);

      else if ( value[key].isArray() == true )
        jsonValidateArray(value[key], valueReference);
    }

    return;
  }

  for ( const auto& key : reference.getMemberNames() )
  {
    if ( value.isMember(key) == false ||
         jsonValidateValue(value[key], reference[key]) == false )
      jsonThrowValidationError(reference[key], key);

    else if ( reference[key].isObject() == true )
      jsonValidateObject(value[key], reference[key]);

    else if ( reference[key].isArray() == true )
      jsonValidateArray(value[key], reference[key]);
  }
}

bool
jsonValidateValue(
  const Json::Value& value,
  const Json::Value& reference )
{
  if ( value.type() == reference.type() )
    return true;

//  treat negative integers as invalid type when reference is uint
  if (  ( reference.isUInt() || reference.isUInt64() ) &&
        value.isConvertibleTo(reference.type()) == true )
    return true;

  return false;
}

Json::Value
jsonParse(
  std::istream& stream )
{
  Json::Value result {};
  Json::String parseErrors {};

  if ( Json::parseFromStream( jsonReader(), stream,
                              &result, &parseErrors ) == false )
  {
    throw std::runtime_error(parseErrors);
  }

  return result;
}

void
jsonClearComments(
  Json::Value& json )
{
  Json::Value tmp {};

  json.swapPayload(tmp);
  json.swap(tmp);

  for ( auto& member : json )
    jsonClearComments(member);
}

Json::Value
jsonClearComments(
  const Json::Value& json )
{
  Json::Value result {json};
  jsonClearComments(result);

  return result;
}


std::string
jsonGetValidationErrorComment(
  const Json::Value& reference,
  const std::string& fallbackPostfix )
{
  const auto comment = reference.getComment(
    Json::CommentPlacement::commentBefore );

  CQDE_ASSERT_DEBUG(comment.size() > 3,
    return "expected different JSON value type" + fallbackPostfix);

  return comment.substr(3);
}

void
jsonThrowValidationError(
  const Json::Value& reference )
{
  throw std::runtime_error(
    jsonGetValidationErrorComment(
      reference, {} ));
}

void
jsonThrowValidationError(
  const Json::Value& reference,
  const std::string& key )
{
  using fmt::format;

  throw std::runtime_error(
    jsonGetValidationErrorComment(
      reference, format(" for key '{}'", key) ));
}

void
jsonThrowValidationError(
  const Json::Value& reference,
  const int index )
{
  using fmt::format;

  throw std::runtime_error(
    jsonGetValidationErrorComment(
      reference, format(" for array element {}", index) ));
}


namespace json_operators
{

Json::Value&
operator << (
  Json::Value& lhs,
  const Json::Value& rhs )
{
  const bool compatibleTypes =
    lhs.type() == rhs.type() &&
    (lhs.isObject() == true || lhs.isArray() == true);

  CQDE_ASSERT_DEBUG(compatibleTypes == true, return lhs);

  if ( lhs.isObject() == true )
    for ( const auto& key : rhs.getMemberNames() )
      lhs[key] = rhs[key];

  else
    for ( const auto& element : rhs )
      lhs.append(element);

  return lhs;
}

Json::Value
operator << (
  const Json::Value& lhs,
  const Json::Value& rhs )
{
  auto result {lhs};
  return result << rhs;
}

} // namespace json_operators

} // namespace cqde
