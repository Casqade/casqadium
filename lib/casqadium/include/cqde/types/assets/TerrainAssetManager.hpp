#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <cqde/types/Terrain.hpp>


namespace cqde::types
{

extern template class
AssetManager <Terrain>;

using TerrainAssetManager = AssetManager <Terrain>;

} // namespace cqde::types
