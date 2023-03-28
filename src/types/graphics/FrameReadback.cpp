#include <cqde/types/graphics/FrameReadback.hpp>

#include <cqde/common.hpp>

#include <entt/entity/entity.hpp>


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
  const glm::u16vec2& pos,
  const glm::u16vec2& size,
  const CallbackFunc& callback )
{
  const auto elementCount = size.x * size.y;

  auto& request = requests.emplace_back();

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

} // namespace cqde::types
