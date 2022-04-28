#pragma once

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

  bool mDoubleSidedFace;

  enum FaceWindingOrder : bool
  {
    CounterClockWise,
    ClockWise

  } mProjectedWindingOrder;

  void drawOutline( const olc::Pixel& ) const;
  void drawSolid( olc::Decal* const,
                  const olc::Pixel& tint ) const;

  bool isClockWise( const bool yAxisUp = false ) const;

public:
  Quad( const std::array <glm::vec3, 4>& = defaultQuadVerts,
        const SceneNode* parent = nullptr );

  void draw( const Camera* ) override;
  void appendCulled(  std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                      const Camera* ) override;

  bool isUnderCursor( olc::vi2d ) const;

  bool isSelected() const;
  void setSelected( const bool );
  void setDoubleSidedFace( const bool );

  void setFrontFace( olc::Decal* );
  void setBackFace( olc::Decal* );
};

} // namespace Graphics3D
