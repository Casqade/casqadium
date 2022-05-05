#pragma once

#include <cqde/alias.hpp>
#include <cqde/common.hpp>

#include <map>


namespace cqde::types
{

template <typename CallbackType>
class CallbackStorage
{
protected:
  std::map <cqde::CallbackId, CallbackType> mCallbacks;

public:
  void Register( const cqde::CallbackId, const CallbackType );
};

} // namespace cqde::types
