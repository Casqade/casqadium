#ifndef RENDER_QUEUE_HPP
#define RENDER_QUEUE_HPP

#include <mutex>
#include <array>
#include <queue>
#include <memory>

#include <AppFramework/RenderCommand.hpp>


class ApplicationCore;

class RenderQueue
{
  static std::mutex mBuffersMutex;
  static std::array <std::queue <std::shared_ptr <RenderCommand>>, 2> mBuffers;

  static std::queue <std::shared_ptr <RenderCommand>>&  mBufferToWrite,
                                                     &  mBufferToRead;

  static std::queue <std::shared_ptr <RenderCommand>>   Read();
  static void SwapBuffers();

  friend class ApplicationCore;

public:
  RenderQueue() = delete;

  static std::queue <std::shared_ptr <RenderCommand>>&  Current();
};


#endif
