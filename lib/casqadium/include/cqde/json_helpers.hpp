#pragma once

#include <json/forwards.h>

#include <iosfwd>


namespace cqde
{

std::string jsonToString(
  const Json::Value&,
  const bool withComments = false );

Json::CharReaderBuilder   jsonReader();
Json::StreamWriterBuilder jsonWriter();

void jsonValidateArray(
  const Json::Value&,
  const Json::Value& reference );

void jsonValidateObject(
  const Json::Value&,
  const Json::Value& reference );

bool jsonValidateValue(
  const Json::Value&,
  const Json::Value& reference );

Json::Value jsonParse( std::istream& stream );

void jsonClearComments( Json::Value& );
Json::Value jsonClearComments( const Json::Value& );


std::string jsonGetValidationErrorComment(
  const Json::Value& reference,
  const std::string& fallbackPostfix );

void jsonThrowValidationError(
  const Json::Value& reference );

void jsonThrowValidationError(
  const Json::Value& reference,
  const std::string& key );

void jsonThrowValidationError(
  const Json::Value& reference,
  const int index );

namespace json_operators
{

// merge json objects/arrays
Json::Value& operator << ( Json::Value&, const Json::Value& );
Json::Value operator << ( const Json::Value&, const Json::Value& );

}

} // namespace cqde
