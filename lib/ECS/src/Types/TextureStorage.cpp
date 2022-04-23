#include <ECS/Types/TextureStorage.hpp>


namespace ECS
{

namespace Types
{

TextureStorage::TextureStorage()
  : textures({{entt::hashed_string("null"), std::make_shared <olc::Renderable> ()}})
{}

} // namespace Types

} // namespace ECS
