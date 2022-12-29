#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>


namespace cqde::types
{

extern template class
AssetManager <olc::Renderable>;

using TextureAssetManager = AssetManager <olc::Renderable>;

} // namespace cqde::types
