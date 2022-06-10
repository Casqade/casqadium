#pragma once

#include <cqde/alias.hpp>

#include <entt/container/dense_map.hpp>
#include <entt/core/utility.hpp>

#include <json/value.h>

#include <thirdparty/ctpl/ctpl_stl.h>


namespace cqde::types
{

template <typename Resource>
class ResourceManager
{
public:
  ResourceManager() = default;

  enum class ResourceStatus
  {
    Undefined,
    Unloaded,
    Loading,
    Loaded,
    Error,
  };

  void parseRegistry( const Json::Value& registry );

  virtual void load( const std::set <cqde::identifier>& );
  virtual void unload( const cqde::identifier& ) const;

  ResourceStatus status( const cqde::identifier& ) const;
  std::shared_ptr <Resource> get( const cqde::identifier& ) const;

private:
  Json::Value mRegistry;
  entt::dense_map < cqde::identifier, cqde::identifier,
                    entt::identity> mPaths {};

  entt::dense_map < cqde::identifier, std::pair <ResourceStatus, std::shared_ptr <Resource>>,
                    entt::identity> mResources {};

  ctpl::thread_pool mJobs {4};
  std::recursive_mutex mResourcesMutex {};
};

} // namespace cqde::types
