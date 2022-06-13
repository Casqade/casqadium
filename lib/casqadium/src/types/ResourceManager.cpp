#include <cqde/types/ResourceManager.hpp>

#include <cqde/util/logger.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>


namespace cqde::types
{

template <typename Resource>
void
ResourceManager <Resource>::parseRegistry(
  const Json::Value& registry )
{
  std::lock_guard guard(mResourcesMutex);

  for ( const auto& id : registry.getMemberNames())
    mIdMap[id] = registry[id].asString();
}

template <typename Resource>
void
ResourceManager <Resource>::load(
  const std::set <cqde::identifier>& ids )
{
  mJobs.push(
  [this, &ids] ( const int32_t threadId )
  {
    for ( const auto& id : ids )
    {
      mResourcesMutex.lock();

      if ( status(id) == ResourceStatus::Loading )
      {
        mResourcesMutex.unlock();
        return;
      }

      mResources.at(id).first = ResourceStatus::Loading;
      mResourcesMutex.unlock();

      // load impl //
      auto ptr = std::make_shared <Resource> ();
      //

      std::lock_guard guard(mResourcesMutex);

      if ( status(id) == ResourceStatus::Unloaded )
      {
        // cleanup ?
        return;
      }

      auto& [status, handle] = mResources[mIdMap[id]];

      status = ResourceStatus::Loaded;
      handle = std::move(ptr);
    }
  });
}

template <typename Resource>
void
ResourceManager <Resource>::unload(
  const cqde::identifier& id )
{
  std::lock_guard guard(mResourcesMutex);

  auto& [status, handle] = mResources.at(cqde::identifier(mIdMap.at(id)));
  status = ResourceStatus::Unloaded;
  handle.reset();
}

template <typename Resource>
void
ResourceManager <Resource>::create(
  const cqde::identifier& id,
  const ResourcePtr res )
{
  std::lock_guard guard(mResourcesMutex);

  auto& [status, handle] = mResources[id];
  status = ResourceStatus::Loaded;
  handle = res;

  mIdMap[id] = id;
}

template <typename Resource>
typename ResourceManager <Resource>::ResourceStatus
ResourceManager <Resource>::status(
  const cqde::identifier& id ) const
{
  std::lock_guard guard(mResourcesMutex);

  if ( mIdMap.count(id) == 0 )
    return ResourceStatus::Undefined;

  if ( mResources.count(mIdMap.at(id)) == 0 )
    return ResourceStatus::Unloaded;

  const auto& [status, ptr] = mResources.at(mIdMap.at(id));
  return status;
}

template <typename Resource>
std::shared_ptr <Resource>
ResourceManager <Resource>::get(
  const cqde::identifier& id ) const
{
  std::lock_guard guard(mResourcesMutex);

  const auto& [status, ptr] = mResources.at(mIdMap.at(id));
  return ptr;
}

} // namespace cqde::types

template class
cqde::types::ResourceManager <olc::Font>;
