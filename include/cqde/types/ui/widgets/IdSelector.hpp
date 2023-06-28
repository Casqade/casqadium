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

  std::string mLabel {"##idSelector"};

public:
  using UserFilter = std::function <bool(const identifier&)>;
  using UserCallback = std::function <void(const identifier&)>;
  using ItemComparator = std::function <bool(const identifier&)>;


  IdSelector(
    const std::string& filterHint,
    const std::string& label );


  bool select(
    const std::vector <identifier>& idList,
    const UserCallback&,
    const UserFilter& = {},
    const ItemComparator& = {} );

  bool selectCombo(
    identifier& selectedId,
    const std::vector <identifier>& idList,
    const UserFilter& = {} );

  bool selectCombo(
    const identifier& selectedId,
    const std::vector <identifier>& idList,
    const UserCallback&,
    const UserFilter& = {} );

  bool selectPopup(
    const std::vector <identifier>& idList,
    const UserCallback&,
    const UserFilter& = {} );
};

} // namespace cqde::ui
