#pragma once

#include <cqde/types/assets/AssetManager.hpp>

#include <glm/vec3.hpp>

#include <vector>


namespace cqde::types
{

extern template class
AssetManager <std::vector <glm::vec3>>;

using GeometryAssetManager = AssetManager <std::vector <glm::vec3>>;

} // namespace cqde::types
