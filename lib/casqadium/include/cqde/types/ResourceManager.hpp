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
  using ResourceId = cqde::identifier;
  using ResourcePath = cqde::identifier;
  using ResourcePtr = std::shared_ptr <Resource>;

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
  virtual void unload( const cqde::identifier& );
  virtual void create( const cqde::identifier&, const ResourcePtr );

  ResourceStatus status( const cqde::identifier& ) const;
  std::shared_ptr <Resource> get( const cqde::identifier& ) const;

private:
  std::map <ResourceId, ResourcePath> mIdMap {};

  std::map <ResourcePath,
            std::pair <ResourceStatus, ResourcePtr>> mResources {};

  ctpl::thread_pool mJobs {4};
  mutable std::recursive_mutex mResourcesMutex {};
};

} // namespace cqde::types
