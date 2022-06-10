#include <cqde/types/ResourceManager.hpp>


namespace cqde::types
{

template <typename Resource>
void
ResourceManager <Resource>::parseRegistry(
  const Json::Value& registry )
{
  std::lock_guard guard(mResourcesMutex);

  for ( const auto& id : registry.getMemberNames())
  {
    mRegistry[id] = registry[id];
    mPaths.insert_or_assign(id, registry[id].asString());
  }
}

template <typename Resource>
void
ResourceManager <Resource>::load(
  const std::set <cqde::identifier>& ids )
{
  mJobs.push(
  [this, &ids]
  {
    for ( const auto& id : ids )
    {
      if ( status(id) == ResourceStatus::Loading )
        return;

      mResourcesMutex.lock();
      mResources.at(id).first = ResourceStatus::Loading;
      mResourcesMutex.unlock();

      auto ptr = std::make_shared <Resource> ();
      // load //

      std::lock_guard guard(mResourcesMutex);

      if ( status(id) == ResourceStatus::Unloaded )
      {
        // cleanup ?
        return;
      }

      auto& [status, handle] = mResources[mPaths[id]];

      status = ResourceStatus::Loaded;
      handle = std::move(ptr);
    }
  });
}

template <typename Resource>
void
ResourceManager <Resource>::unload(
  const cqde::identifier& id ) const
{
  std::lock_guard guard(mResourcesMutex);

  auto& [status, handle] = mResources.at(cqde::identifier(mPaths.at(id)));
  status = ResourceStatus::Unloaded;
  handle.reset();
}

template <typename Resource>
typename ResourceManager <Resource>::ResourceStatus
ResourceManager <Resource>::status(
  const cqde::identifier& id ) const
{
  std::lock_guard guard(mResourcesMutex);

  if ( mPaths.contains(id) == false )
    return ResourceStatus::Undefined;

  if ( mResources.contains(mPaths.at(id)) == false )
    return ResourceStatus::Unloaded;

  return mResources.at(cqde::identifier(mPaths.at(id))).first;
}

template <typename Resource>
std::shared_ptr <Resource>
ResourceManager <Resource>::get(
  const cqde::identifier& id ) const
{
  std::lock_guard guard(mResourcesMutex);
  return mResources.at(cqde::identifier(mPaths.at(id))).second;
}

} // namespace cqde::types
