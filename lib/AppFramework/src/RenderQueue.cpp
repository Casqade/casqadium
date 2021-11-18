#include <TimeUtils/Duration.hpp>

#include <AppFramework/RenderCommand.hpp>
#include <AppFramework/RenderQueue.hpp>
#include <AppFramework/TickTimestamp.hpp>


std::mutex RenderQueue::mBuffersMutex = {};

std::array <std::queue <std::shared_ptr <RenderCommand>>, 2>
  RenderQueue::mBuffers = {};

std::queue <std::shared_ptr <RenderCommand>>&
  RenderQueue::mBufferToWrite = RenderQueue::mBuffers.front();
std::queue <std::shared_ptr <RenderCommand>>&
  RenderQueue::mBufferToRead = RenderQueue::mBuffers.back();


void
RenderQueue::SwapBuffers()
{
  std::lock_guard <std::mutex> guard(mBuffersMutex);
  std::swap( mBufferToRead, mBufferToWrite );
  mBufferToWrite = {};
  mBufferToWrite.push(std::make_shared <RenderCommand> (TickTimestamp( TimeUtils::Now() )));
}

std::queue <std::shared_ptr <RenderCommand>>&
RenderQueue::Current()
{
  return mBufferToWrite;
}

std::queue <std::shared_ptr <RenderCommand>>
RenderQueue::Read()
{
  std::lock_guard <std::mutex> guard(mBuffersMutex);
  return mBufferToRead;
}
