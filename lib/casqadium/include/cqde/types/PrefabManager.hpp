#pragma once

#include <cqde/alias.hpp>

#include <json/value.h>

#include <filesystem>


namespace cqde::ui
{
class PrefabManagerUi;
}

namespace cqde::types
{

class PrefabManager
{
  friend class ui::PrefabManagerUi;

  using path = std::filesystem::path;

  Json::Value mPrefabs {};


  void deserialize( const Json::Value& );

public:
  PrefabManager() = default;

  void load( const path& );
  void clear();

  Json::Value serialize() const;

  static void Validate( const Json::Value& );

  Json::Value get( const PrefabId& ) const;
};

} // namespace cqde::types
