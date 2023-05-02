#pragma once

#include <soloud_queue.h>


namespace cqde::types
{

class SoloudQueue : public SoLoud::Queue
{
public:
  void clear();
  void clearPending();
};

} // namespace cqde::types
