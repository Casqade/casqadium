#include <cqde/components/EntityMetaInfo.hpp>

#include <json/value.h>


namespace cqde::compos
{

Json::Value
EntityMetaInfo::serialize() const
{
  return Json::objectValue;
}

void
EntityMetaInfo::deserialize(
  entt::registry&,
  entt::entity,
  const Json::Value& )
{}

} // namespace cqde::compos
