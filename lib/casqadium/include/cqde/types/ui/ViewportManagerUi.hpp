#pragma once

#include <cqde/alias.hpp>

#include <cqde/types/EntityReference.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/fwd.hpp>

#include <map>


namespace cqde::ui
{

class ViewportManagerUi
{
  using EntityReference = types::EntityReference;

  std::map <EntityReference, bool> mViewports {};

  StringFilter mCameraFilter {"Camera entity ID"};

public:
  ViewportManagerUi() = default;

  bool mouseOverViewport( const EntityId& cameraId ) const;

  void ui_show( entt::registry& );
  void ui_show_viewport_windows( entt::registry& );
};

} // namespace cqde::ui
