#pragma once

#include <cqde/identifier.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/fwd.hpp>

#include <vector>


namespace cqde::ui
{

class IdSelector
{
  StringFilter mIdFilter {"ID"};

  std::string mComboLabel {"##idSelectorComboId"};

public:
  IdSelector(
    const std::string& filterHint,
    const std::string& comboLabel );

  bool select(
    const entt::registry& registry,
    identifier& selectedId,
    const std::vector <identifier>& idList );
};

} // namespace cqde::ui
