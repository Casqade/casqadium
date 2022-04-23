#include <ECS/Systems/CullingSystem.hpp>

#include <ECS/Components/Camera.hpp>
#include <ECS/Components/GeometryBuffer.hpp>
#include <ECS/Components/Transform.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Types/VertexBuffer.hpp>

#include <entt/entt.hpp>


namespace ECS
{

namespace Systems
{

ECS::Types::VertexBuffer
vertexShader(
  const ECS::Components::GeometryBuffer& cGeometryBuffer,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport )
{
  bool offScreen = true;

  ECS::Types::VertexBuffer vertexBuffer{};
  vertexBuffer.vertices.reserve(cGeometryBuffer.vertexes.size());

  for ( const glm::vec3& srcVert : cGeometryBuffer.vertexes )
  {
    const glm::vec3 vert =
      glm::projectZO( srcVert,
                      modelViewMatrix,
                      projectionMatrix,
                      viewport );

    vertexBuffer.vertices.push_back({ vert.x, viewport.w - vert.y });
    vertexBuffer.depth += vert.z;

    if ( vert.z < 0.0f || vert.z > 1.0f )
      return {{}, -1.0f};

    if (    (vert.x >= viewport.x && vert.y >= viewport.y)
         && (vert.x <= viewport.z && vert.y <= viewport.w) )
      offScreen = false;
  }

//  if ( offScreen )
//    return {{}, -1.0f};

  vertexBuffer.windingOrder = ECS::Types::GetWindingOrder(vertexBuffer);

  vertexBuffer.depth /= vertexBuffer.vertices.size();

  return vertexBuffer;
}


void CullSystem( entt::registry& registry )
{
  using namespace ECS::Components;

  for ( const auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform>().each() )
  {
    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewport;

    for ( const auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
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

} // namespace Systems

} // namespace ECS
