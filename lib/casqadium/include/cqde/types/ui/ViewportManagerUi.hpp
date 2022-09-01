#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/EntityReference.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/fwd.hpp>

#include <vector>


namespace cqde::ui
{

class ViewportManagerUi
{
  using EntityReference = types::EntityReference;

  std::vector <types::EntityReference> mViewports {};

  StringFilter mCameraFilter {"Camera entity ID"};

  int32_t viewportIndex( const EntityId& cameraId ) const;

public:
  ViewportManagerUi() = default;

  bool hasViewport( const EntityId& cameraId ) const;
  bool mouseOverViewport( const EntityId& cameraId ) const;

  void ui_show( entt::registry& );
  void ui_show_viewport_windows( entt::registry& );
};

} // namespace cqde::ui
