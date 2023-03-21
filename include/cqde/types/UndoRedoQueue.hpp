#pragma once

#include <deque>
#include <cstddef>


namespace cqde::types
{

template <typename T>
class UndoRedoQueue
{
  using QueueIterator = typename std::deque <T>::iterator;

  std::deque <T> mQueue {};
  QueueIterator mQueueIter {};

  size_t mQueueLength {};

public:
  UndoRedoQueue( const size_t queueLength );

  void push( const T& element );

  T undo();
  T redo();

  bool undoAvailable() const;
  bool redoAvailable() const;

  QueueIterator current() const;
  bool isValid ( const QueueIterator ) const;
};

}
