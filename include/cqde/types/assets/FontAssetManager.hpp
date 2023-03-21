#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <olcPGE/Font.hpp>


namespace cqde::types
{

extern template class
AssetManager <olc::Font>;

using FontAssetManager = AssetManager <olc::Font>;

} // namespace cqde::types
