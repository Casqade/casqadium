#include <cqde/systems/CullingSystem.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <cqde/types/VertexBuffer.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <entt/entt.hpp>


namespace cqde::systems
{

static cqde::types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport )
{
  using cqde::types::VertexBuffer;

  if ( vertices.size() == 0 )
    return {{}, -1.0f, {}};

  bool offScreen = true;

  VertexBuffer vb{};
  vb.vertices.reserve(vertices.size());

  for ( const glm::vec3& srcVert : vertices )
  {
    const glm::vec3 vert =
      glm::project( srcVert,
                    modelViewMatrix,
                    projectionMatrix,
                    viewport );

    vb.vertices.push_back({ vert.x, // converting y axis to top-left origin
                            viewport.y * 2 + viewport.w - vert.y });
    vb.depth += vert.z;

    if ( vert.z < 0.0f || vert.z > 1.0f )
      return {{}, -1.0f, {}};

    if (    (vert.x >= viewport.x && vert.y >= viewport.y)
         && (vert.x <= viewport.x + viewport.z && vert.y <= viewport.y + viewport.w) )
      offScreen = false;
  }

//  if ( offScreen )
//    return {{}, -1.0f};

  vb.windingOrderUpdate();

  vb.depth /= vb.vertices.size();

  return vb;
}


void CullingSystem( entt::registry& registry )
{
  using namespace cqde::compos;
  using cqde::types::GeometryAssetManager;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform> ().each() )
  {
    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewport;

    for ( const auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
            : registry.view <GeometryBuffer, SceneNode, Transform>().each() )
    {
      const auto gBuffer = geometry.try_get(cGeometryBuffer.buffer);
      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView *
                                  cqde::GetWorldMatrix(registry, cTransform, cNode);

      auto vBuffer = vertexShader( *gBuffer,
                                  modelView,
                                  camProjection,
                                  camViewport );

      if ( vBuffer.depth < 0.0f )
        continue;

      cCamera.zBuffer.emplace( vBuffer, eDrawable );
    }
  }
}

} // namespace cqde::systems
