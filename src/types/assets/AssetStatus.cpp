#include <cqde/types/assets/AssetStatus.hpp>

#include <cqde/assert.hpp>

#include <imgui.h>


namespace cqde::types
{

std::string
AssetStatusAsString(
  const AssetStatus status )
{
  switch (status)
  {
    case AssetStatus::Undefined:
      return "Undefined";

    case AssetStatus::Unloaded:
      return "Unloaded";

    case AssetStatus::Loading:
      return "Loading";

    case AssetStatus::Loaded:
      return "Loaded";

    case AssetStatus::Error:
      return "Error";

    default:
      CQDE_ASSERT_DEBUG(false, return "Undefined");
  }
}

uint32_t
AssetStatusAsColor(
  const AssetStatus status )
{
  switch (status)
  {
    case AssetStatus::Undefined:
      return ImGui::ColorConvertFloat4ToU32({1.0f, 1.0f, 0.0f, 1.0f});

    case AssetStatus::Unloaded:
      return ImGui::ColorConvertFloat4ToU32({1.0f, 1.0f, 1.0f, 1.0f});

    case AssetStatus::Loading:
      return ImGui::ColorConvertFloat4ToU32({0.0f, 1.0f, 1.0f, 1.0f});

    case AssetStatus::Loaded:
      return ImGui::ColorConvertFloat4ToU32({0.0f, 1.0f, 0.0f, 1.0f});

    case AssetStatus::Error:
      return ImGui::ColorConvertFloat4ToU32({1.0f, 0.0f, 0.0f, 1.0f});

    default:
      CQDE_ASSERT_DEBUG(false, return ImGui::ColorConvertFloat4ToU32({1.0f, 1.0f, 1.0f, 1.0f}););
  }
}

} // namespace cqde::types
