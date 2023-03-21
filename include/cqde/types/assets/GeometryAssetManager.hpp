#pragma once

#include <cqde/types/assets/AssetManager.hpp>
#include <cqde/types/VertexBuffer.hpp>


namespace cqde::types
{

extern template class
AssetManager <VertexBuffer>;

using GeometryAssetManager = AssetManager <VertexBuffer>;

} // namespace cqde::types
