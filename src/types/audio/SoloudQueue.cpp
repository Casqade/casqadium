#include <cqde/types/audio/SoloudQueue.hpp>


namespace cqde::types
{

void
SoloudQueue::clear()
{
  if ( mSoloud == nullptr )
    return;

  mSoloud->lockAudioMutex_internal();

  mCount = 0;
  mReadIndex = 0;
  mWriteIndex = 0;

  for ( int i = 0; i < SOLOUD_QUEUE_MAX; ++i )
    if ( mSource[i] != nullptr )
    {
      delete mSource[i];
      mSource[i] = nullptr;
    }

  mSoloud->unlockAudioMutex_internal();
}

void
SoloudQueue::clearPending()
{
  if ( mSoloud == nullptr )
    return;

  mSoloud->lockAudioMutex_internal();

  for ( int i = 0; i < SOLOUD_QUEUE_MAX; ++i )
    if ( i != mReadIndex &&
         mSource[i] != nullptr )
    {
      delete mSource[i];
      mSource[i] = nullptr;
    }

  mCount = 1;
  mWriteIndex = (mReadIndex + 1) % SOLOUD_QUEUE_MAX;

  mSoloud->unlockAudioMutex_internal();
}

} // namespace cqde::types
