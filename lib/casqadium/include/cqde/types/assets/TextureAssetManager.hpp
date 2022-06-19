#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>


namespace cqde::types
{

extern template class
AssetManager <olc::Renderable>;

typedef AssetManager <olc::Renderable> TextureAssetManager;

} // namespace cqde::types
