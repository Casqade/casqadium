#pragma once

#include <cqde/types/UndoRedoQueue.hpp>


namespace cqde::types
{

template <typename T>
UndoRedoQueue <T>::UndoRedoQueue(
  const size_t queueLength )
  : mQueueIter {mQueue.end()}
  , mQueueLength{queueLength}
{}

template <typename T>
void
UndoRedoQueue <T>::push(
  const T& element )
{
  if ( mQueueIter != mQueue.end() &&
       element == *mQueueIter )
    return;

  if ( mQueue.empty() == false &&
       mQueueIter + 1 < mQueue.end() )
    mQueue.erase(mQueueIter + 1, mQueue.end());

  mQueue.push_back(element);

  if ( mQueue.size() > mQueueLength )
    mQueue.pop_front();

  mQueueIter = mQueue.end() - 1;
}

template <typename T>
T
UndoRedoQueue <T>::undo()
{
  if ( undoAvailable() == true )
    return *--mQueueIter;

  return {};
}

template <typename T>
T
UndoRedoQueue <T>::redo()
{
  if ( redoAvailable() == true )
    return *++mQueueIter;

  return {};
}

template <typename T>
bool
UndoRedoQueue <T>::undoAvailable() const
{
  return
    mQueueIter > mQueue.begin() &&
    mQueueIter != mQueue.end();
}

template <typename T>
bool
UndoRedoQueue <T>::redoAvailable() const
{
  if ( mQueue.size() < 2 )
    return false;

  return mQueueIter < mQueue.end() - 1;
}

template <typename T>
typename UndoRedoQueue <T>::QueueIterator
UndoRedoQueue <T>::current() const
{
  return mQueueIter;
}

template <typename T>
bool
UndoRedoQueue <T>::isValid (
  const QueueIterator iter ) const
{
  return iter < mQueue.end();
}

}
