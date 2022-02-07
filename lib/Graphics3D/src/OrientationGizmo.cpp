#include <Graphics3D/OrientationGizmo.hpp>
#include <Graphics3D/Camera.hpp>


namespace Graphics3D
{

OrientationGizmo::OrientationGizmo( Camera* cam )
  : mCamera(cam)
  , mVerts({{ {{ {-0.5f, -0.5f, -0.5f}, {0.5f, -0.5f, -0.5f} }},
              {{ {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f} }},
              {{ {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f, 0.5f} }} }})
  , mVertsProjected()
{}

void
OrientationGizmo::draw()
{
  olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[0][0],
                                        mVertsProjected[0][1],
                                        olc::RED);

  olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[1][0],
                                        mVertsProjected[1][1],
                                        olc::GREEN);

  olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[2][0],
                                        mVertsProjected[2][1],
                                        olc::BLUE);

  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[0][1], {4.0f, 4.0f}, olc::RED );
  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[1][1], {4.0f, 4.0f}, olc::GREEN );
  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[2][1], {4.0f, 4.0f}, olc::BLUE );
}

void
OrientationGizmo::appendCulled(
  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
  const Camera* camera )
{
  SceneNode::appendCulled( depthBuffer, camera );
  glm::mat4 model = glm::toMat4(glm::inverse(camera->orientation()))
                  * glm::scale( glm::mat4(1.0f), mScale );

  const auto windowSize = olc::renderer->ptrPGE->GetWindowSize();

  const Camera gizmoCam(Camera::Projection::Orthogonal, {},
                        {0.0f, 0.0f, 200.0f, 200.0f});

  const glm::mat4 modelView = glm::translate( glm::mat4(1.0f), {0.0f, 0.0f, -5.0f} ) * model;
  const glm::mat4 projection = gizmoCam.projMatrix();
  const glm::vec4 viewport = gizmoCam.viewport();

  for ( size_t axis = 0;
        axis < mVerts.size();
        ++axis )
    for ( size_t i = 0;
          i < mVerts[axis].size();
          ++i )
    {
      const glm::vec3 vert
        = glm::projectZO( mVerts[axis][i],
                          modelView,
                          projection,
                          viewport );

      mVertsProjected[axis][i] = { viewport.z * 0.5f + vert.x, viewport.w * 0.5f - vert.y };
    }

  depthBuffer.emplace(0.0f, this);
}

void
OrientationGizmo::setCamera( Camera* cam )
{
  mCamera = cam;
}

} // namespace Graphics3D
