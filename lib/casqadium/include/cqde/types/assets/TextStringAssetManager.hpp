#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <string>


namespace cqde::types
{

extern template class
AssetManager <std::string>;

typedef AssetManager <std::string> TextStringAssetManager;

} // namespace cqde::types
