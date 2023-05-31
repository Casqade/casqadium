#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <cqde/types/MouseCursor.hpp>


namespace cqde::types
{

extern template class
AssetManager <MouseCursor>;

using MouseCursorAssetManager = AssetManager <MouseCursor>;

} // namespace cqde::types
