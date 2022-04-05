#include <GameStateEcsSandbox.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <ECS/Common.hpp>
#include <ECS/Components/Camera.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Transform.hpp>
#include <ECS/Components/VertexBuffer.hpp>
#include <ECS/Components/GeometryBuffer.hpp>


ECS::Components::VertexBuffer::WindingOrder
GetWindingOrder(
  const std::vector <olc::vf2d>& vertexes,
  const bool yAxisUp = false )
{
  float area = {};

  for ( size_t i = 0, iNext = 1;
        i < vertexes.size();
        ++i, iNext = (i + 1) % vertexes.size() )
    area += (vertexes[iNext].x - vertexes[i].x)
          * (vertexes[iNext].y + vertexes[i].y);

  return ECS::Components::VertexBuffer::WindingOrder(yAxisUp ? area > 0.0f : area < 0.0f);
}

ECS::Components::VertexBuffer
vertexShader(
  const ECS::Components::GeometryBuffer& cGeometryBuffer,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport )
{
  bool  offScreen = true;

  ECS::Components::VertexBuffer vertexBuffer{};

  for ( size_t i = 0;
        i < cGeometryBuffer.vertexes.size();
        ++i )
  {
    const glm::vec3 vert =
      glm::projectZO( cGeometryBuffer.vertexes[i],
                      modelViewMatrix,
                      projectionMatrix,
                      viewport );

    vertexBuffer.vertexes[i] = { vert.x, viewport.w - vert.y };
    vertexBuffer.depth += vert.z;

    if ( vert.z < 0.0f || vert.z > 1.0f )
      return {{}, -1.0f};

    if (    (vert.x >= viewport.x && vert.y >= viewport.y)
         && (vert.x <= viewport.z && vert.y <= viewport.w) )
      offScreen = false;
  }

  if ( offScreen )
    return {{}, -1.0f};

  vertexBuffer.windingOrder = GetWindingOrder(vertexBuffer.vertexes);

  vertexBuffer.depth /= vertexBuffer.vertexes.size();

  return vertexBuffer;
}

void draw( entt::registry& registry )
{
  using namespace ECS::Components;

  for ( auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform>().each() )
  {
    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewport;

    for ( auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
            : registry.view <GeometryBuffer, SceneNode, Transform>().each() )
    {
      const glm::mat4 modelView = camView *
                                  ECS::GetWorldMatrix(registry, cTransform, cNode) *
                                  cTransform.modelLocal();

      auto buffer = vertexShader( cGeometryBuffer,
                                  modelView,
                                  camProjection,
                                  camViewport );

      if ( buffer.depth < 0.0f )
        continue;

      cCamera.zBuffer.emplace( buffer, eDrawable );
    }
  }
}

std::array <olc::vf2d, 4>
vec_to_array( const std::vector <olc::vf2d>& src )
{
  std::array <olc::vf2d, 4> dest;
  std::copy( src.begin(), src.begin() + 4, dest.begin() );

  return std::move(dest);
}

void
RenderSystem( entt::registry& registry )
{
  using namespace ECS::Components;

  for ( auto&& [eCamera, cCamera] : registry.view <Camera>().each() )
  {
    for ( auto& [buffer, entity] : cCamera.zBuffer )
    {
      olc::renderer->ptrPGE->DrawWarpedDecal( nullptr, vec_to_array(buffer.vertexes) );
    }
  }
}


GameStateEcsSandbox::GameStateEcsSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
{
  using namespace entt::literals;

  entt::registry reg;
  ECS::registryInit(reg);

  auto entity = reg.create();

  auto resolved = entt::resolve("Transform"_hs);
  auto any = resolved.construct();
  auto invoked = any.invoke("deserialze"_hs, std::ref(reg), entity, std::string("newcontent"));

  std::cout << bool(resolved) << bool(any) << bool(invoked) << "\n";

  for(const auto &&[ent, comp] : reg.view <ECS::Components::Transform>().each())
  {
    std::cout << comp.translation.x << "\n";
  }
  std::cout << "size " << reg.size() << reg.size <ECS::Components::Transform> () << "\n";
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  return false;
}
