#include <ECS/Components/TextureBuffer.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Components
{

TextureBuffer::TextureBuffer()
{
  textures.reserve(2);  // front+back faces
}

} // namespace Components

} // namespace ECS
