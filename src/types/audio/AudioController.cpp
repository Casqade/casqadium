#include <cqde/types/audio/AudioController.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/physics/RigidBody.hpp>

#include <cqde/ecs_helpers.hpp>
#include <cqde/json_helpers.hpp>
#include <cqde/conversion/rp3d_glm.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/EntityManager.hpp>

#include <entt/entity/registry.hpp>

#include <reactphysics3d/body/RigidBody.h>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>


namespace cqde::types
{

//const static Json::Value audioFaderJsonReference =
//[]
//{
//  using ValueType = Json::ValueType;
//  using namespace std::string_literals;

//  Json::Value root = ValueType::objectValue;
//  root.setComment("// root must be a JSON object"s,
//                   Json::CommentPlacement::commentBefore);

//  auto& from = root["from"];
//  from = ValueType::realValue;
//  from.setComment("// 'from' must be a JSON float"s,
//                  Json::CommentPlacement::commentBefore);

//  auto& to = root["to"];
//  to = ValueType::realValue;
//  to.setComment("// 'to' must be a JSON float"s,
//                Json::CommentPlacement::commentBefore);

//  auto& delta = root["delta"];
//  delta = ValueType::realValue;
//  delta.setComment("// 'delta' must be a JSON float"s,
//                    Json::CommentPlacement::commentBefore);

//  auto& time = root["time"];
//  time = ValueType::realValue;
//  time.setComment("// 'time' must be a JSON double"s,
//                  Json::CommentPlacement::commentBefore);

//  auto& startTime = root["startTime"];
//  startTime = ValueType::realValue;
//  startTime.setComment("// 'startTime' must be a JSON double"s,
//                        Json::CommentPlacement::commentBefore);

//  auto& endTime = root["endTime"];
//  endTime = ValueType::realValue;
//  endTime.setComment("// 'endTime' must be a JSON double"s,
//                      Json::CommentPlacement::commentBefore);

//  auto& current = root["current"];
//  current = ValueType::realValue;
//  current.setComment("// 'current' must be a JSON float"s,
//                      Json::CommentPlacement::commentBefore);

//  auto& active = root["active"];
//  active = ValueType::intValue;
//  active.setComment("// 'active' must be a JSON integer"s,
//                    Json::CommentPlacement::commentBefore);

//  return root;
//}();

//Json::Value&
//operator << (
//  Json::Value& json,
//  const SoLoud::Fader& fader )
//{
//  json["from"] = fader.mFrom;
//  json["to"] = fader.mTo;
//  json["delta"] = fader.mDelta;
//  json["time"] = fader.mTime;
//  json["startTime"] = fader.mStartTime;
//  json["endTime"] = fader.mEndTime;
//  json["current"] = fader.mCurrent;
//  json["active"] = fader.mActive;

//  return json;
//}

//SoLoud::Fader&
//operator << (
//  SoLoud::Fader& fader,
//  const Json::Value& json )
//{
//  jsonValidateObject(json, audioFaderJsonReference);

//  fader.mFrom = json["from"].asFloat();
//  fader.mTo = json["to"].asFloat();
//  fader.mDelta = json["delta"].asFloat();
//  fader.mTime = json["time"].asDouble();
//  fader.mStartTime = json["startTime"].asDouble();
//  fader.mEndTime = json["endTime"].asDouble();
//  fader.mCurrent = json["current"].asFloat();
//  fader.mActive = json["active"].asInt();

//  return fader;
//}

//Json::Value&
//operator << (
//  Json::Value& json,
//  const SoLoud::FilterInstance& filter )
//{
////  json["type"] = filter.getType();

//  auto& jsonParams = json["params"];
//  jsonParams = Json::arrayValue;

//  for ( size_t i = 0;
//        i < filter.mNumParams;
//        ++i )
//  {
//    Json::Value jsonParam {};

//    jsonParam["value"] = filter.mParam[i];
//    jsonParam["fader"] << filter.mParamFader[i];

//    jsonParams.append(jsonParam);
//  }

//  return json;
//}

//SoLoud::FilterInstance&
//operator << (
//  SoLoud::FilterInstance& filter,
//  const Json::Value& json )
//{
//  using fmt::format;

//  size_t i {};
//  for ( const auto& jsonParam : json["params"] )
//  {
//    if ( i >= filter.mNumParams )
//      break;

//    filter.mParam[i] = jsonParam["value"].asFloat();

//    for ( const auto& jsonParamFader : jsonParam["fader"] )
//      try
//      {
//        filter.mParamFader[i] << jsonParamFader;
//      }
//      catch ( const std::exception& e )
//      {
//        throw std::runtime_error(
//          format("fader {} parse error: {}",
//                 i, e.what()));
//      }

//    ++i;
//  }

//  return filter;
//}

} // namespace cqde::types
