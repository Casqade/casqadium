#include <cqde/systems/RenderSystem.hpp>

#include <cqde/common.hpp>
#include <cqde/components/Camera.hpp>
#include <cqde/components/TextureBuffer.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entt.hpp>


std::array <olc::vf2d, 4>
vec_to_array( const std::vector <olc::vf2d>& src )
{
  std::array <olc::vf2d, 4> dest;
  std::copy( src.begin(), src.begin() + 4, dest.begin() );

  return dest;
}

namespace cqde::systems
{

void
RenderSystem( entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  for ( const auto&& [eCamera, cCamera] : registry.view <Camera> ().each() )
  {
    for ( const auto& [buffer, entity] : cCamera.zBuffer )
    {
      auto vertices = vec_to_array(buffer.vertices);

      if ( cCamera.textureMode == Camera::TextureMode::Textured )
      {
        const TextureBuffer* textureBuffer = registry.try_get <TextureBuffer> (entity);
        if ( textureBuffer != nullptr )
        {
          if ( (textureBuffer->textures.size() > 0 &&
                buffer.windingOrder == VertexBuffer::WindingOrder::CounterClockWise) ||
               (textureBuffer->textures.size() > 1 &&
                buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise) )
          {
            auto& textures  = registry.ctx().at <TextureAssetManager> ();
            const TextureId textureId = textureBuffer->textures.at((int) buffer.windingOrder);

            const auto  texture = textures.get(textureId);
            olc::Decal* decal = texture ?
                                texture->Decal() : nullptr;

            if ( decal != nullptr )
            {
              if ( buffer.windingOrder == VertexBuffer::WindingOrder::ClockWise )
                std::reverse(vertices.begin(), vertices.end());

              olc::renderer->ptrPGE->DrawWarpedDecal( decal, vertices );
            }
          }
        }
      }

      if ( cCamera.renderMode == Camera::RenderMode::Solid )
        continue;

//      Draw outline
      for ( size_t i = 0, iNext = 1;
            i < vertices.size();
            ++i, iNext = (i + 1) % vertices.size() )
        olc::renderer->ptrPGE->DrawLineDecal( vertices[i],
                                              vertices[iNext],
                                              olc::CYAN );
    }
  }
}

} // namespace cqde::systems
