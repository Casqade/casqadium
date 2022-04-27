#pragma once

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>

#include <map>


namespace ECS
{

namespace Types
{

template <typename CallbackType>
class CallbackStorage
{
protected:
  std::map <ECS::CallbackId, CallbackType,
            entt::hashed_string_comparator> mCallbacks;

public:
  void Register( const ECS::CallbackId, const CallbackType );
};

} // namespace Types

} // namespace ECS
