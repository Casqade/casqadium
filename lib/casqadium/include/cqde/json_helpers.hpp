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

Json::Value jsonParse( std::istream& stream );

} // namespace cqde
