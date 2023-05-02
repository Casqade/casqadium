#include <cqde/types/graphics/FrameReadback.hpp>

#include <cqde/assert.hpp>
#include <cqde/types/graphics/RenderTarget.hpp>
#include <cqde/types/CallbackManager.hpp>

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

#include <cstring>


namespace cqde::types
{

FrameReadbackQueue::~FrameReadbackQueue()
{
  while ( requests.empty() == false )
    pop();
}

FrameReadbackQueue::FrameReadbackQueue(
  FrameReadbackQueue&& other )
{
  *this = std::move(other);
}

FrameReadbackQueue&
FrameReadbackQueue::operator = (
  FrameReadbackQueue&& other )
{
  requests = std::move(other.requests);
  other.requests.clear();

  return *this;
}

FrameReadbackRequest&
FrameReadbackQueue::push(
  const GLuint framebufferId,
  const glm::u16vec2& pos,
  const glm::u16vec2& size,
  const CallbackFunc& callback )
{
  const auto elementCount = size.x * size.y;

  auto& request = requests.emplace_back();

  request.framebufferId = framebufferId;
  request.pos = pos;
  request.size = size;
  request.data.resize(elementCount, entt::null);
  request.callback = callback;

  const auto bufferSize =
    elementCount * sizeof(decltype(request.data)::value_type);

  request.buffers.first.create(
    bufferSize, 0 );

  request.buffers.second.create(
    bufferSize,
    GL_MAP_READ_BIT | GL_CLIENT_STORAGE_BIT );

  return request;
}

void
FrameReadbackQueue::pop()
{
  CQDE_ASSERT_DEBUG(requests.empty() == false, return);

  auto& request = requests.front();
  auto& buffers = request.buffers;

  buffers.first.destroy();
  buffers.second.destroy();

  if ( request.fence != nullptr )
  {
    glClientWaitSync( request.fence,
      GL_SYNC_FLUSH_COMMANDS_BIT,
      std::numeric_limits <GLuint64>::max() );

    glDeleteSync(request.fence);
  }

  requests.pop_front();
}

void
FrameReadbackQueue::clear()
{
  while ( requests.empty() == false )
    pop();
}

void
FrameReadbackQueue::write(
  const types::RenderTarget& target )
{
  for ( auto& request : requests )
    if ( request.fence == nullptr &&
         request.framebufferId == target.fbo )
    {
      glBindBuffer(GL_PIXEL_PACK_BUFFER, request.buffers.first.id());

      glNamedFramebufferReadBuffer(
        target.fbo,
        GL_COLOR_ATTACHMENT1 );

      glReadPixels(
        request.pos.x, request.pos.y,
        request.size.x, request.size.y,
        GL_RED_INTEGER,
        GL_UNSIGNED_INT,
        nullptr );

      const auto bufferSize =
        sizeof(decltype(request.data)::value_type) * request.size.x * request.size.y;

      glCopyNamedBufferSubData(
        request.buffers.first.id(),
        request.buffers.second.id(),
        0, 0,
        bufferSize );

      request.fence = glFenceSync(
        GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
    }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void
FrameReadbackQueue::process(
  entt::registry& registry )
{
  auto& callbackManager = registry.ctx().get <CallbackManager> ();

  while ( requests.empty() == false )
  {
    auto& request = requests.front();

    if ( request.fence == nullptr )
      break;

    GLint status {};
    GLsizei length {};

    glGetSynciv( request.fence,
      GL_SYNC_STATUS, sizeof(status),
      &length, &status );

    if ( status != GL_SIGNALED )
      break;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, request.buffers.first.id());

    const auto readDataSize =
      sizeof(decltype(request.data)::value_type) * request.size.x * request.size.y;

    const auto readData = request.buffers.second.map(
      0, readDataSize,
      GL_MAP_READ_BIT );

    std::memcpy(
      request.data.data(),
      readData, readDataSize );

    request.buffers.second.unmap();

    if ( request.callback != nullptr )
    {
      const FrameReadbackResult result
      {
        request.pos,
        request.size,
        std::move(request.data),
      };

      callbackManager.executeLater(
        request.callback, {result} );
    }

    pop();
  }

  glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

} // namespace cqde::types
