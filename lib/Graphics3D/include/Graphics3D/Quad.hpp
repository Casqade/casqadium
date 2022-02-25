#ifndef GRAPHICS_3D_QUAD_HPP
#define GRAPHICS_3D_QUAD_HPP

#include <olcPGE/olcPixelGameEngine.hpp>

#include <Graphics3D/SceneNode.hpp>


namespace Graphics3D
{

constexpr std::array <glm::vec3, 4> defaultQuadVerts =
{{
  {-0.5f, 0.5f, 0.0f},
  {-0.5f, -0.5f, 0.0f},
  {0.5f, -0.5f, 0.0f},
  {0.5f, 0.5f, 0.0f}
}};

class Quad : public SceneNode
{
  std::array <glm::vec3, 4> mVerts;
  std::array <olc::vf2d, 4> mVertsProjected;

  bool mSelected;

  olc::Decal* mFrontFaceDecal;
  olc::Decal* mBackFaceDecal;

  olc::Pixel  mFrontFaceColor;
  olc::Pixel  mBackFaceColor;

  enum FaceWindingOrder : bool
  {
    CounterClockWise,
    ClockWise

  } mProjectedWindingOrder;

  void drawOutline( const olc::Pixel& ) const;

  bool isClockWise( const bool yAxisUp = false ) const;

public:
  Quad( const std::array <glm::vec3, 4>& = defaultQuadVerts,
        const SceneNode* parent = nullptr );

  void draw() override;
  void appendCulled(  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                      const Camera* ) override;

  bool isUnderCursor( olc::vi2d ) const;

  bool isSelected() const;
  void setSelected( bool );

  void setFrontFace( olc::Decal* );
  void setFrontFace( const olc::Pixel );

  void setBackFace( olc::Decal* );
  void setBackFace( const olc::Pixel );
};

}
#endif
