#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <soloud.h>


namespace cqde::types
{

extern template class
AssetManager <SoLoud::AudioSource>;

using AudioAssetManager = AssetManager <SoLoud::AudioSource>;

} // namespace cqde::types
