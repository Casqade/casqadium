#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <entt/entity/entity.hpp>

#include <json/forwards.h>

#include <map>
#include <filesystem>


namespace cqde::types
{

class EntityManager
{
  std::map <EntityId, entt::entity> tags {{null_id, entt::null}};

public:
  EntityManager() = default;

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
