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
  {
    const std::string comment = reference.getComment(Json::CommentPlacement::commentBefore);
    CQDE_ASSERT_DEBUG(comment.size() > 3, throw std::runtime_error("expected different JSON value type"));
    throw std::runtime_error(comment.substr(3));
  }

  if ( reference.size() == 0 )
    return;

//  todo: perform check only for first element ?
  for ( uint32_t i = 0;
        i < array.size();
        ++i )
    if ( array[i].type() != reference.begin()->type() )
    {
      if ( ( reference.begin()->isUInt() || reference.begin()->isUInt64() ) &&
            array[i].isConvertibleTo(reference.begin()->type()) == true )
        continue;

      const std::string comment = reference.begin()->getComment(Json::CommentPlacement::commentBefore);
      CQDE_ASSERT_DEBUG(comment.size() > 3, throw std::runtime_error(format("expected different JSON value type for array element {}", i)));
      throw std::runtime_error(comment.substr(3));
    }

    else if ( array[i].isObject() == true )
      jsonValidateObject( array[i], *reference.begin() );

    else if ( array[i].isArray() == true )
      jsonValidateArray( array[i], *reference.begin() );
}

void
jsonValidateObject(
  const Json::Value& value,
  const Json::Value& reference )
{
  using fmt::format;

  if ( value.type() != reference.type() )
  {
    const std::string comment = reference.getComment(Json::CommentPlacement::commentBefore);
    CQDE_ASSERT_DEBUG(comment.size() > 3, throw std::runtime_error("expected different JSON value type"));
    throw std::runtime_error(comment.substr(3));
  }

  if ( reference.isMember("cqde_json_anykey") == true )
  {
    const auto valueReference = reference["cqde_json_anykey"];

    for ( const auto& key : value.getMemberNames() )
    {
      if ( value[key].type() != valueReference.type() )
      {
        const std::string comment = valueReference.getComment(Json::CommentPlacement::commentBefore);
        CQDE_ASSERT_DEBUG(comment.size() > 3, throw std::runtime_error(format("expected different JSON value type for key '{}'", key)));
        throw std::runtime_error(comment.substr(3));
      }

      if ( value[key].isObject() == true )
        jsonValidateObject(value[key], valueReference);
      else if ( value[key].isArray() == true )
        jsonValidateArray(value[key], valueReference);
    }

    return;
  }

  for ( const auto& key : reference.getMemberNames() )
    if ( value[key].type() != reference[key].type() )
    {
//      treat negative integers as invalid type when reference is uint
      if (  value.isMember(key) == true &&
            ( reference[key].isUInt() || reference[key].isUInt64() ) &&
            value[key].isConvertibleTo(reference[key].type()) == true )
        continue;

      const std::string comment = reference[key].getComment(Json::CommentPlacement::commentBefore);
      CQDE_ASSERT_DEBUG(comment.size() > 3, throw std::runtime_error(format("expected different JSON value type for key '{}'", key)));
      throw std::runtime_error(comment.substr(3));
    }

    else if ( reference[key].isObject() == true )
      jsonValidateObject(value[key], reference[key]);

    else if ( reference[key].isArray() == true )
      jsonValidateArray(value[key], reference[key]);
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

} // namespace cqde
