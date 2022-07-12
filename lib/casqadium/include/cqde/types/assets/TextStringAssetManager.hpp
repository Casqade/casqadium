#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <string>


namespace cqde::types
{

extern template class
AssetManager <std::string>;

using TextStringAssetManager = AssetManager <std::string>;

} // namespace cqde::types
