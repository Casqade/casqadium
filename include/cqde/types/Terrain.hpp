#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>


namespace cqde::types
{

struct Terrain
{
//  height field
  std::vector <float> data { {}, {}, {}, {} };

  struct
  {
    int32_t rows {2};
    int32_t columns {2};

  } grid {};

  std::pair <float, float> heightRange {};


  Terrain() = default;
};

} // namespace cqde::types
