#pragma once

#include <cqde/fwd.hpp>
#include <cqde/types/CallbackStorage.hpp>

#include <functional>


namespace cqde::types
{

class InputCallbackStorage : public cqde::types::CallbackStorage <std::function <void(const entt::entity, cqde::compos::InputController&)>>
{
public:
  void Execute( const cqde::CallbackId,
                const entt::entity,
                cqde::compos::InputController& );
};

} // namespace cqde::types
