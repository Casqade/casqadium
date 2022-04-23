#include <ECS/Systems/RenderSystem.hpp>

#include <ECS/Common.hpp>
#include <ECS/Components/Camera.hpp>
#include <ECS/Components/TextureBuffer.hpp>
#include <ECS/Types/TextureStorage.hpp>

#include <entt/entt.hpp>


std::array <olc::vf2d, 4>
vec_to_array( const std::vector <olc::vf2d>& src )
{
  std::array <olc::vf2d, 4> dest;
  std::copy( src.begin(), src.begin() + 4, dest.begin() );

  return std::move(dest);
}

namespace ECS
{

namespace Systems
{

void
RenderSystem( entt::registry& registry )
{
  using namespace ECS::Components;
  using namespace ECS::Types;

  for ( auto&& [eCamera, cCamera] : registry.view <Camera>().each() )
  {
    for ( auto& [buffer, entity] : cCamera.zBuffer )
    {
      olc::Decal* decal = nullptr;

      TextureBuffer* textureBuffer = registry.try_get <TextureBuffer> (entity);
      if ( textureBuffer != nullptr )
      {
          const auto      textureId = textureBuffer->textures[(int) buffer.windingOrder];
          const TextureStorage& textureStorage = registry.ctx <TextureStorage> ();

          decal = textureStorage.textures.at(textureId)->Decal();
      }

      auto vertices = vec_to_array(buffer.vertices);
      olc::renderer->ptrPGE->DrawWarpedDecal( decal, vec_to_array(buffer.vertices) );

      for ( size_t i = 0, iNext = 1;
            i < vertices.size();
            ++i, iNext = (i + 1) % vertices.size() )
        olc::renderer->ptrPGE->DrawLineDecal( vertices[i],
                                              vertices[iNext],
                                              olc::CYAN );
    }
  }
}

} // namespace Systems

} // namespace ECS
