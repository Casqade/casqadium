#ifndef GRAPHICS_3D_ORIENTATION_GIZMO_HPP
#define GRAPHICS_3D_ORIENTATION_GIZMO_HPP

#include <olcPGE/olcPixelGameEngine.hpp>

#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

class OrientationGizmo : public SceneNode
{
  Camera* mCamera;

  std::array <std::array <glm::vec3, 2>, 3> mVerts;
  std::array <std::array <olc::vf2d, 2>, 3> mVertsProjected;

public:
  OrientationGizmo( Camera* = nullptr );

  void draw() override;
  void appendCulled(  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                      const Camera* ) override;

  void setCamera( Camera* );
};

}

#endif
