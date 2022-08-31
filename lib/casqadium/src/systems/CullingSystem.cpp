#include <cqde/systems/CullingSystem.hpp>

#include <cqde/math_helpers.hpp>

#include <cqde/components/Camera.hpp>
#include <cqde/components/GeometryBuffer.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/components/SceneNode.hpp>

#include <cqde/types/VertexBuffer.hpp>
#include <cqde/types/assets/GeometryAssetManager.hpp>

#include <entt/entt.hpp>


namespace cqde::systems
{

types::VertexBuffer
vertexShader(
  const std::vector <glm::vec3>& vertices,
  const glm::mat4& modelViewMatrix,
  const glm::mat4& projectionMatrix,
  const glm::vec4& viewport )
{
  using types::VertexBuffer;

  if ( vertices.size() == 0 )
    return {{}, -1.0f, {}};

  VertexBuffer vb{};
  vb.vertices.reserve(vertices.size());

  for ( const glm::vec3& srcVert : vertices )
  {
    const glm::vec3 vertex =
      glm::project( srcVert,
                    modelViewMatrix,
                    projectionMatrix,
                    viewport );

    if ( vertex.z < 0.0f || vertex.z > 1.0f )
      return {{}, -1.0f, {}};

    vb.vertices.push_back({ vertex.x, // converting y axis to top-left origin
                            viewport.y * 2 + viewport.w - vertex.y });
    vb.depth += vertex.z;
  }

  const Rect bb = boundingBox(vb.vertices);

  const Rect vp
  {
    .left = viewport.x,
    .right = viewport.x + viewport.z,
    .top = viewport.y,
    .bottom = viewport.y + viewport.w
  };

  if ( rectsIntersect(bb, vp) == false )
    return {{}, -1.0f};

  vb.depth /= vb.vertices.size();

  vb.windingOrderUpdate();

  return vb;
}


void CullingSystem(
  entt::registry& registry )
{
  using namespace compos;
  using types::GeometryAssetManager;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform> ().each() )
  {
    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    const glm::vec4 camViewport = cCamera.viewportScaled();

    for ( const auto&& [eDrawable, cGeometryBuffer, cNode, cTransform]
            : registry.view <GeometryBuffer, SceneNode, Transform>().each() )
    {
      const auto gBuffer = geometry.get(cGeometryBuffer.buffer);

      if ( gBuffer == nullptr )
        continue;

      const glm::mat4 modelView = camView * GetWorldMatrix(registry, cTransform, cNode);

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
