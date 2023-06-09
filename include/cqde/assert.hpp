#pragma once

#include <cassert>


// assert in debug build
// handle condition safely in release
#define CQDE_ASSERT_DEBUG(cond, action)\
  {if ( (cond) ); else { assert(cond); action; }}
