#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <olcPGE/olcPGEX_TTF.hpp>


namespace cqde::types
{

extern template class
AssetManager <olc::Font>;

typedef AssetManager <olc::Font> FontAssetManager;

} // namespace cqde::types
