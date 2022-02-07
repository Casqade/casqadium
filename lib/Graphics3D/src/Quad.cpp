#include <Graphics3D/Quad.hpp>
#include <Graphics3D/Camera.hpp>


namespace Graphics3D
{

bool wireFrameEnabled = false;

bool frontFaceWindingOrder = false;

olc::Pixel wireFrameFrontFaceColor = olc::DARK_RED;
olc::Pixel wireFrameBackFaceColor = olc::DARK_BLUE;


///
/// Vertices must be counter-clockwise ordered
/// counting from topleft corner.
///
Quad::Quad(
  const std::array <glm::vec3, 4>& verts,
  const SceneNode* parent )
  : SceneNode(parent)
  , mVerts(verts)
  , mVertsProjected()
  , mSelected()
  , mFrontFaceDecal()
  , mBackFaceDecal()
  , mFrontFaceColor(wireFrameFrontFaceColor)
  , mBackFaceColor(wireFrameBackFaceColor)
{}

bool
Quad::isClockWise( const bool yAxisUp ) const
{
  float area = {};

  for ( size_t i = 0, iNext = 1;
        i < mVertsProjected.size();
        ++i, iNext = (i + 1) % mVertsProjected.size() )
    area += (mVertsProjected[iNext].x - mVertsProjected[i].x)
          * (mVertsProjected[iNext].y + mVertsProjected[i].y);

  return yAxisUp ? area > 0.0f : area < 0.0f;
}

void
Quad::appendCulled( std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
                    const Camera* camera )
{
  SceneNode::appendCulled( depthBuffer, camera );

  const glm::mat4 modelView = camera->viewMatrix() * modelWorld();
  const glm::mat4 projection = camera->projMatrix();
  const glm::vec4 viewport = camera->viewport();

  bool  offScreen = true;
  float polygonDepth = 0.0f;

  for ( size_t i = 0;
        i < mVerts.size();
        ++i )
  {
    const glm::vec3 vert
      = glm::projectZO( mVerts[i],
                        modelView,
                        projection,
                        viewport );

    mVertsProjected[i] = { vert.x, viewport.w - vert.y };
    polygonDepth += vert.z;

    if ( vert.z < 0.0f || vert.z > 1.0f )
      return;

    if (    (vert.x >= 0.0f && vert.y >= 0.0f)
         && (vert.x <= viewport.z && vert.y <= viewport.w) )
      offScreen = false;
  }

  if ( offScreen )
    return;

  mProjectedWindingOrder = (FaceWindingOrder) isClockWise();
//  if (    wireFrameEnabled == false
//       && mBackFaceDecal == nullptr
//       && mProjectedWindingOrder != frontFaceWindingOrder
//          )
//    return;

  polygonDepth /= mVertsProjected.size();
  depthBuffer.emplace( polygonDepth, this );
}

void
Quad::draw()
{
  if ( wireFrameEnabled )
  {
    if ( mSelected )
      drawOutline(  mProjectedWindingOrder == frontFaceWindingOrder ?
                    wireFrameFrontFaceColor.inv() : wireFrameBackFaceColor.inv() );
    else
      drawOutline(  mProjectedWindingOrder == frontFaceWindingOrder ?
                    wireFrameFrontFaceColor : wireFrameBackFaceColor );

    olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[2], { 3.0f, 3.0f }, olc::RED );
    olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[0], { 3.0f, 3.0f }, olc::GREEN );
    olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[1], { 3.0f, 3.0f }, olc::BLUE );

    return;
  }

  if ( mProjectedWindingOrder == frontFaceWindingOrder )
    olc::renderer->ptrPGE->DrawWarpedDecal( mFrontFaceDecal,
                                            mVertsProjected,
                                            mFrontFaceDecal ? olc::WHITE : mFrontFaceColor );
  else
    olc::renderer->ptrPGE->DrawWarpedDecal( mBackFaceDecal,
                                            mVertsProjected,
                                            mBackFaceDecal ? olc::WHITE : mBackFaceColor );

  if ( mSelected )
    drawOutline(  mProjectedWindingOrder == frontFaceWindingOrder ?
                  wireFrameFrontFaceColor.inv() : wireFrameBackFaceColor.inv() );
}

void
Quad::drawOutline( const olc::Pixel& color ) const
{
  for ( size_t i = 0, iNext = 1;
        i < mVertsProjected.size();
        ++i, iNext = (i + 1) % mVertsProjected.size() )
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[i],
                                          mVertsProjected[iNext],
                                          color );
}

bool
Quad::isUnderCursor( olc::vi2d mouse ) const
{
  bool edgeCrossed = false;

  for ( int i = 0, j = mVertsProjected.size() - 1;
        i < mVertsProjected.size();
        j = i++ )
  {
    if ( ( (mVertsProjected[i].y > mouse.y) != (mVertsProjected[j].y > mouse.y) ) &&
     (mouse.x < (mVertsProjected[j].x - mVertsProjected[i].x)
      * (mouse.y - mVertsProjected[i].y)
      / (mVertsProjected[j].y - mVertsProjected[i].y) + mVertsProjected[i].x) )
       edgeCrossed = !edgeCrossed;
  }

  return edgeCrossed;
}

void
Quad::setSelected( bool selected )
{
  mSelected = selected;
}

void
Quad::setFrontFace( olc::Decal* decal )
{
  mFrontFaceDecal = decal;
}

void
Quad::setFrontFace( const olc::Pixel color )
{
  mFrontFaceColor = color;
}

void
Quad::setBackFace( olc::Decal* decal )
{
  mBackFaceDecal = decal;
}

void
Quad::setBackFace( const olc::Pixel color )
{
  mBackFaceColor = color;
}

}
