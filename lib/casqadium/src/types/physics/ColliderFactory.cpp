#include <cqde/types/physics/ColliderFactory.hpp>


namespace cqde::types
{

std::shared_ptr <Collider>
ColliderFactory::create(
  const std::string& id ) const
{
  if ( mColliders.count(id) > 0 )
    return mColliders.at(id)();

  return {};
}

std::vector <std::string>
ColliderFactory::colliders() const
{
  std::vector <std::string> result {};
  result.reserve(mColliders.size());

  for ( const auto& [id, creator] : mColliders )
    result.push_back(id);

  return result;
}

} // namespace cqde::types
