#include <Widgets.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#include <Graphics3D/Camera.hpp>


namespace olc
{

Text2D::Text2D(
  const olc::CustomFont* font,
  const std::string& text,
  const olc::Pixel color,
  const bool monospaced )
  : mFont(font)
  , mText(text)
  , mColor(color)
  , mPos()
  , mSize(monospaced ? font->GetTextSize(text) : font->GetTextSizeProp(text))
  , mRotation()
  , mMonospaced(monospaced)
{}

void
Text2D::render() const
{
  if ( mText.empty() )
    return;

  if ( mMonospaced )
    return olc::DrawRotatedFontDecal( mPos, mFont, mText,
                                      mRotation, {},
                                      mColor, scale() );

  olc::DrawRotatedFontPropDecal( mPos, mFont, mText,
                                 mRotation, {},
                                 mColor, scale() );
}

void
Text2D::setFont( const CustomFont* font )
{
  mFont = font;
  mSize = mMonospaced ? mFont->GetTextSize(mText) : mFont->GetTextSizeProp(mText);
}

void
Text2D::setText( const std::string& text )
{
  mText = text;
  mSize = mMonospaced ? mFont->GetTextSize(mText) : mFont->GetTextSizeProp(mText);
}

void
Text2D::setColor( const olc::Pixel color )
{
  mColor = color;
}

void
Text2D::setMonospaced( const bool monospaced )
{
  mMonospaced = monospaced;
  mSize = mMonospaced ? mFont->GetTextSize(mText) : mFont->GetTextSizeProp(mText);
}

void
Text2D::setPos( const vf2d pos )
{
  mPos = pos;
}

void
Text2D::setSize( const vu2d size )
{
  mSize = size;
}

void
Text2D::setOrigin( const vf2d origin )
{
  mPos = origin - mSize / 2;
}

void
Text2D::setRotation( const float angle )
{
  mRotation = angle;
}

const olc::CustomFont*
Text2D::font() const
{
  return mFont;
}

std::string
Text2D::text() const
{
  return mText;
}

olc::Pixel
Text2D::color() const
{
  return mColor;
}

vf2d
Text2D::pos() const
{
  return mPos;
}

vu2d
Text2D::size() const
{
  return mSize;
}

vu2d
Text2D::textSize() const
{
  return mFont->GetTextSize(mText);
}

vf2d
Text2D::scale() const
{
  return mText.empty() ? vf2d(1.0f, 1.0f) : mMonospaced ? mSize / mFont->GetTextSize(mText) : mSize / mFont->GetTextSizeProp(mText);
}

vf2d
Text2D::origin() const
{
  return mPos + mSize / 2.0f;
}

float
Text2D::rotation() const
{
  return mRotation;
}

}


namespace Graphics3D
{

bool wireFrameEnabled = false;

bool frontFaceWindingOrder = false;

olc::Pixel wireFrameFrontFaceColor = olc::DARK_RED;
olc::Pixel wireFrameBackFaceColor = olc::DARK_BLUE;

///
/// Vertices must be counter-clockwise ordered
/// and begin from topleft corner.
///
Poly3D::Poly3D(
  const std::array <glm::vec3, 4>& verts,
  const SceneNode* parent )
  : SceneNode(parent)
  , mVerts(verts)
  , mVertsProjected()
  , mFrontFaceDecal()
  , mBackFaceDecal()
  , mFrontFaceColor(wireFrameFrontFaceColor)
  , mBackFaceColor(wireFrameBackFaceColor)
{}

bool
Poly3D::isClockWise( const bool yAxisUp ) const
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
Poly3D::appendCulled( std::multimap < float, SceneNode*, std::greater <float>>& depthBuffer,
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
//       && mProjectedWindingOrder != frontFaceWindingOrder )
//    return;

  polygonDepth /= mVertsProjected.size();
  depthBuffer.emplace( polygonDepth, this );
}

void
Poly3D::draw()
{
  for ( auto& child : mChildren )
  {
    Poly3D* poly = dynamic_cast <Poly3D*> ( child.get() );
    if ( poly )
      poly->draw();
  }

  if ( wireFrameEnabled == false )
  {
    if ( mProjectedWindingOrder == frontFaceWindingOrder )
    {
      if ( mFrontFaceDecal != nullptr )
        return olc::renderer->ptrPGE->DrawWarpedDecal( mFrontFaceDecal, mVertsProjected );
      else
        return olc::renderer->ptrPGE->DrawWarpedDecal( mFrontFaceDecal, mVertsProjected, mFrontFaceColor );
    }

    if ( mBackFaceDecal != nullptr )
      return olc::renderer->ptrPGE->DrawWarpedDecal( mBackFaceDecal, mVertsProjected );

    return olc::renderer->ptrPGE->DrawWarpedDecal( mBackFaceDecal, mVertsProjected, mBackFaceColor );
  }

  for ( size_t i = 0, iNext = 1;
        i < mVertsProjected.size();
        ++i, iNext = (i + 1) % mVertsProjected.size() )
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[i],
                                          mVertsProjected[iNext],
                                          mProjectedWindingOrder == frontFaceWindingOrder ?
                                          wireFrameFrontFaceColor : wireFrameBackFaceColor );

  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[2], { 3.0f, 3.0f }, olc::RED );
  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[0],{ 3.0f, 3.0f }, olc::GREEN );
  olc::renderer->ptrPGE->FillRectDecal( mVertsProjected[1], { 3.0f, 3.0f }, olc::BLUE );
}

void
Poly3D::setFrontFace( olc::Decal* decal )
{
  mFrontFaceDecal = decal;
}

void
Poly3D::setFrontFace( const olc::Pixel color )
{
  mFrontFaceColor = color;
}

void
Poly3D::setBackFace( olc::Decal* decal )
{
  mBackFaceDecal = decal;
}

void
Poly3D::setBackFace( const olc::Pixel color )
{
  mBackFaceColor = color;
}

} // namespace Graphics3D


InputPrompt::InputPrompt(
  const std::string& prompt,
  const olc::CustomFont* font,
  const TimeUtils::Duration durationShown,
  const TimeUtils::Duration durationHidden )
  : olc::Text2D(font, "[" + prompt + "]")
  , mState(State::Inactive)
  , mDurationShown(durationShown)
  , mDurationHidden(durationHidden)
  , mTimeInState({}, true)
{}

void
InputPrompt::update( const TimeUtils::Duration dt )
{
  mTimeInState.update(dt);

  if ( mTimeInState.isReady() == false )
    return;

  switch ( mState )
  {
    case State::Shown:
      return setState( State::Hidden );

    case State::Hidden:
      return setState( State::Shown );

    default:
      return;
  }
}

void
InputPrompt::setState( const State state )
{
  mState = state;

  switch ( mState )
  {
    case State::Inactive:
    {
      olc::Pixel fillColor = this->color();
      fillColor.a = 0;

      return setColor( fillColor );
    }

    case State::Shown:
    {
      olc::Pixel fillColor = this->color();
      fillColor.a = 255;
      setColor( fillColor );

      return mTimeInState.start( mDurationShown );
    }

    case State::Hidden:
    {
      olc::Pixel fillColor = this->color();
      fillColor.a = 0;
      setColor( fillColor );

      return mTimeInState.start( mDurationHidden );
    }

    default:
      return;
  }
}

bool
InputPrompt::isActive() const
{
  return mState != State::Inactive;
}

FadeEffect::FadeEffect(
  const olc::RectF              rect,
  const std::pair < olc::Pixel,
                    olc::Pixel> fadeRange,
  const TimeUtils::Duration     fadeDuration )
  : olc::RectF(rect)
  , mFadeTimer(fadeDuration)
  , mFadeRange(fadeRange)
{}

void
FadeEffect::update( const TimeUtils::Duration dt )
{
  if ( mFadeTimer.isReady() )
    return;

  const double coef =
    std::min( double(mFadeTimer.duration() - mFadeTimer.update(dt))
                / (double) mFadeTimer.duration(),
              1.0 );

  mFadeColor.r
    = mFadeRange.first.r
    + (float(mFadeRange.second.r) - mFadeRange.first.r)
    * coef;

  mFadeColor.g
    = mFadeRange.first.g
    + (float(mFadeRange.second.g) - mFadeRange.first.g)
    * coef;

  mFadeColor.b
    = mFadeRange.first.b
    + (float(mFadeRange.second.b) - mFadeRange.first.b)
    * coef;

  mFadeColor.a
    = mFadeRange.first.a
    + (float(mFadeRange.second.a) - mFadeRange.first.a)
    * coef;
}

void
FadeEffect::setFadeRange(
  std::pair <olc::Pixel, olc::Pixel> fadeRange )
{
  mFadeRange = fadeRange;
  mFadeTimer.restart();
}

void
FadeEffect::setFadeDuration(
  TimeUtils::Duration fadeDuration )
{
  mFadeTimer.start( fadeDuration );
}

olc::Pixel
FadeEffect::fadeColor() const
{
  return mFadeColor;
}

bool
FadeEffect::finished() const
{
  return mFadeTimer.isReady();
}
