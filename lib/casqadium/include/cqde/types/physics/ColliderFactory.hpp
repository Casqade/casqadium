#pragma once

#include <cqde/types/physics/colliders/Collider.hpp>

#include <functional>
#include <unordered_map>


namespace cqde::types
{

class ColliderFactory
{
  using ColliderCreator = std::function <std::shared_ptr <Collider> ()>;

  std::unordered_map <std::string, ColliderCreator> mColliders {};

public:
  ColliderFactory() = default;

  template <class T>
  void registerCollider( const std::string& id );

  std::shared_ptr <Collider> create( const std::string& id ) const;

  std::vector <std::string> colliders() const;
};

template <class T>
void
ColliderFactory::registerCollider(
  const std::string& id )
{
  mColliders[id] = std::make_shared <T>;
}

} // namespace cqde::types
