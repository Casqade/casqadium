#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <olcPGE/Renderable.hpp>


namespace cqde::types
{

extern template class
AssetManager <olc::Renderable>;

using TextureAssetManager = AssetManager <olc::Renderable>;

} // namespace cqde::types
