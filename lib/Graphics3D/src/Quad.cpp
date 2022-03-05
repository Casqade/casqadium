#include <Graphics3D/Quad.hpp>
#include <Graphics3D/Camera.hpp>


namespace Graphics3D
{

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
  , mDoubleSidedFace()
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

  if ( mDoubleSidedFace == false
       && mProjectedWindingOrder != frontFaceWindingOrder )
    return;

  polygonDepth /= mVertsProjected.size();
  depthBuffer.emplace( polygonDepth, this );
}

void
Quad::draw( const Camera* camera )
{
  olc::Pixel colorLight = olc::WHITE;
  olc::Pixel colorTexture = olc::WHITE;

  switch ( camera->renderMode() )
  {
    case Camera::RenderMode::Wireframe:
    {
      colorTexture =  mProjectedWindingOrder == frontFaceWindingOrder ?
                      wireFrameFrontFaceColor : wireFrameBackFaceColor;

      if ( mSelected )
        drawOutline(  mProjectedWindingOrder == frontFaceWindingOrder ?
                      wireFrameFrontFaceColor.inv() : wireFrameBackFaceColor.inv() );
      else
        drawOutline( colorTexture );

      olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[2], { 5.0f, 5.0f }, olc::RED );
      olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[0], { 5.0f, 5.0f }, olc::GREEN );
      olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[1], { 5.0f, 5.0f }, olc::BLUE );

      break;
    }

    default:
      break;
  }

  switch ( camera->lightingMode() )
  {
    case Camera::LightingMode::Diffuse:
    {
      colorLight = olc::WHITE;

      break;
    }

    default:
      break;
  }

  switch ( camera->textureMode() )
  {
    case Camera::TextureMode::Color:
    {
      drawSolid( nullptr, colorTexture );

      break;
    }

    case Camera::TextureMode::Textured:
    {
      drawSolid(  mProjectedWindingOrder == frontFaceWindingOrder ?
                    mFrontFaceDecal : mBackFaceDecal,
                  colorLight );

      break;
    }

    default:
      return;
  }

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

void
Quad::drawSolid( olc::Decal* const decal,
                 const olc::Pixel& tint ) const
{
  olc::renderer->ptrPGE->DrawWarpedDecal( decal,
                                          mVertsProjected,
                                          tint );
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
Quad::setSelected( const bool selected )
{
  mSelected = selected;
}

void
Quad::setDoubleSidedFace( const bool doubleSidedFace )
{
  mDoubleSidedFace = doubleSidedFace;
}

void
Quad::setFrontFace( olc::Decal* decal )
{
  mFrontFaceDecal = decal;
}

void
Quad::setBackFace( olc::Decal* decal )
{
  mBackFaceDecal = decal;
}

}
