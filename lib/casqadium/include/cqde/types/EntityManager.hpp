#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <json/forwards.h>

#include <map>
#include <filesystem>


namespace cqde::types
{

class EntityManager
{
  std::map <EntityId, entt::entity> tags;

public:
  EntityManager();

  void loadRegistry(  const std::filesystem::path&,
                      const std::string& packageTitle,
                      entt::registry& );

  void saveRegistry( const std::filesystem::path&,
                     const PackageId&,
                     entt::registry& );

  entt::entity get( const EntityId& ) const;
  EntityId createId( EntityId hint = {"e_"} ) const;
};

} // namespace cqde::types
