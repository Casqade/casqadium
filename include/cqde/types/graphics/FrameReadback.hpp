#pragma once

#include <cqde/types/CallbackManager.hpp>
#include <cqde/types/graphics/GlBuffer.hpp>

#include <glm/vec2.hpp>

#include <glad/gl.h>

#include <list>


namespace cqde::types
{

struct FrameReadbackRequest
{
  using CallbackFunc = CallbackManager::CallbackFunc;

  glm::u16vec2 pos {};
  glm::u16vec2 size {};

  std::vector <uint32_t> data {};

  CallbackFunc callback {};

  std::pair <GlBuffer, GlBuffer> buffers {};
  GLsync fence {};

  GLuint framebufferId {};


  FrameReadbackRequest() = default;
};

struct FrameReadbackResult
{
  glm::u16vec2 pos {};
  glm::u16vec2 size {};

  std::vector <uint32_t> data {};
};

struct FrameReadbackQueue
{
  using CallbackFunc = FrameReadbackRequest::CallbackFunc;

  std::list <FrameReadbackRequest> requests {};


  FrameReadbackQueue() = default;
  ~FrameReadbackQueue();

  FrameReadbackQueue( FrameReadbackQueue&& );
  FrameReadbackQueue& operator = ( FrameReadbackQueue&& );


  FrameReadbackRequest& push(
    const GLuint framebufferId,
    const glm::u16vec2& pos,
    const glm::u16vec2& size,
    const CallbackFunc& );

  void pop();
  void clear();

  void write( const RenderTarget& );
  void process( entt::registry& );
};

} // namespace cqde::types
