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

  struct
  {
    float left, right {};
    float top, bottom {};

  } bb {};

  bool bb_initialzed {};

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

    const auto& vb_vertex = vb.vertices.back();

    if ( bb_initialzed == false )
    {
      bb_initialzed = true;
      bb =
      {
        vb_vertex.x, vb_vertex.x,
        vb_vertex.y, vb_vertex.y,
      };
    }

    if ( vb_vertex.x < bb.left )
      bb.left = vb_vertex.x;

    else if ( vb_vertex.x > bb.right )
      bb.right = vb_vertex.x;

    if ( vb_vertex.y < bb.top )
      bb.top = vb_vertex.y;

    else if ( vb_vertex.y > bb.bottom )
      bb.bottom = vb_vertex.y;
  }

  const bool onScreen
  {
    bb.left < viewport.x + viewport.z &&
    bb.right > viewport.x &&
    bb.top < viewport.y + viewport.w &&
    bb.bottom > viewport.y
  };

  if ( onScreen == false )
    return {{}, -1.0f};

  vb.depth /= vb.vertices.size();

  vb.windingOrderUpdate();

  return vb;
}


void CullingSystem( entt::registry& registry )
{
  using namespace cqde::compos;
  using types::GeometryAssetManager;

  auto& geometry = registry.ctx().at <GeometryAssetManager> ();

  for ( const auto&& [eCamera, cCamera, cCameraNode, cCameraTransform]
          : registry.view <Camera, SceneNode, Transform> ().each() )
  {
    cCamera.zBuffer.clear();

    const glm::mat4 camView = cCamera.viewMatrix(registry, cCameraNode, cCameraTransform);
    const glm::mat4 camProjection = cCamera.projMatrix();
    glm::vec4 camViewport = cCamera.viewportScaled();

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
