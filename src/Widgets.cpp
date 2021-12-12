#include <Widgets.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>


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


Camera3D::Camera3D(
  const glm::mat4&  projection,
  const glm::vec4&  viewport,
  const glm::vec3&  origin,
  const glm::quat&  orientation )
  : mProjection(projection)
  , mViewport(viewport)
  , mOrigin(origin)
  , mOrientation(orientation)
  , mSpeed(1.0f)
  , mZoom(45.0f)
{
  recalculateVectors();
}

void
Camera3D::move( const glm::vec3& direction )
{
  mOrigin += direction * mSpeed;

  std::cout << mOrigin.x << " " << mOrigin.y << " " << mOrigin.z << "\n";
}

void
Camera3D::rotate( const glm::quat& rotation )
{
  mOrientation *= rotation;
  mOrientation = glm::normalize(mOrientation);
//  recalculateVectors();
}

void
Camera3D::rotate( const glm::vec4& rotation )
{
  mOrientation = glm::rotate( mOrientation, rotation.w, glm::vec3( rotation ) );
}

void
Camera3D::recalculateVectors()
{
  glm::quat qFront = mOrientation * glm::quat(0, 0, 0, -1) * glm::conjugate(mOrientation);

//  mFront = {qFront.x, qFront.y, qFront.z};
//  mRight = glm::normalize( glm::cross(mFront, glm::vec3{0, 1, 0}) );
//  mUp = glm::normalize( glm::cross(mFront, mRight) );

//  const glm::mat4 transform = glm::inverse( glm::translate(glm::mat4(1.0f), mOrigin) * glm::toMat4(mOrientation) );

//  mFront = glm::normalize( glm::vec3(transform[2]) );
//  mUp = glm::normalize( glm::vec3(transform[1]) );

//  mFront = glm::normalize(glm::rotate(glm::inverse(mOrientation), glm::vec3(0.0, 0.0, 1.0)));
//  mRight = glm::normalize(glm::rotate(glm::inverse(mOrientation), glm::vec3(1.0, 0.0, 0.0)));
//  mUp = glm::normalize(glm::rotate(glm::inverse(mOrientation), glm::vec3(0.0, 1.0, 0.0)));
}

glm::quat
Camera3D::orientation() const
{
  return mOrientation;
}

glm::vec3
Camera3D::front() const
{
  return glm::rotate( mOrientation, {0.0f, 0.0f, -1.0f} );
}

glm::vec3
Camera3D::right() const
{
  return glm::rotate( mOrientation, {1.0f, 0.0f, 0.0f} );
}

glm::vec3
Camera3D::up() const
{
  return glm::rotate( mOrientation, {0.0f, 1.0f, 0.0f} );
}

glm::mat4
Camera3D::viewMatrix() const
{
  return glm::lookAt( mOrigin,
                      mOrigin + front(),
                      up() );
}

glm::mat4
Camera3D::projMatrix() const
{
  return mProjection;
}

glm::vec4
Camera3D::viewport() const
{
  return mViewport;
}


Drawable3D::Drawable3D( const glm::vec3 origin,
                        const glm::quat orientation,
                        const glm::vec3 scale )
  : mOrigin(origin)
  , mOrientation(orientation)
  , mScale(scale)
{}

glm::mat4
Drawable3D::modelMatrix() const
{
//  glm::mat4 translate = glm::translate( glm::mat4(1.0f), mOrigin );
//  glm::mat4 rotation = glm::orientate4( mOrientation );
//  glm::mat4 scale = glm::scale( mScale );

  return  glm::translate( glm::mat4(1.0f), mOrigin )
        * glm::toMat4(mOrientation)
        * glm::scale( glm::mat4(1.0f), mScale );
}

void
Drawable3D::appendCulled( std::multimap < float, Drawable3D*, std::greater <float>>&,
                          const Camera3D& )
{
  return;
}

void
Drawable3D::draw()
{
  return;
}

void
Drawable3D::translate( const glm::vec3 translation )
{
  mOrigin += translation;
}

void
Drawable3D::rotate( const glm::quat rotation )
{
  mOrientation *= rotation;
  std::cout << "pyr:\n";
  std::cout << glm::degrees(glm::pitch(mOrientation)) << "\n";
  std::cout << glm::degrees(glm::yaw(mOrientation)) << "\n";
  std::cout << glm::degrees(glm::roll(mOrientation)) << "\n\n";
}

void
Drawable3D::scale( const glm::vec3 scale )
{
  mScale += scale;
}

void
Drawable3D::setOrigin( const glm::vec3 origin )
{
  mOrigin = origin;
}

void
Drawable3D::setOrientation( const glm::quat orientation )
{
  mOrientation = orientation;
}

void
Drawable3D::setScale( const glm::vec3 scale )
{
  mScale = scale;
}


Poly3D::Poly3D(
  const std::array <glm::vec3, 4>& verts,
  const glm::vec3   origin,
  const glm::quat   orientation,
  const glm::vec3   scale,
  olc::Decal* decal )
  : Drawable3D(origin, orientation, scale)
//  , mNormal(glm::normalize(glm::cross(verts[2] - verts[0], verts[1] - verts[0])))
  , mNormal(glm::normalize(glm::cross(verts[1] - verts[0], verts[2] - verts[0])))
  , mVerts(verts)
  , mVertsProjected()
  , mDecal(decal)
{}

void
Poly3D::appendCulled( std::multimap < float, Drawable3D*, std::greater <float>>& depthBuffer,
                      const Camera3D& cam )
{
  const glm::mat4 modelView = cam.viewMatrix() * modelMatrix();
  const glm::mat4 projection = cam.projMatrix();
  const glm::vec4 viewport = cam.viewport();

  float polygonDepth = 0.0f;

  for ( size_t i = 0;
        i < mVerts.size();
        ++i )
  {
    glm::vec3 vert = glm::projectZO(  mVerts[i],
                                      modelView,
                                      projection,
                                      viewport );

    mVertsProjected[i] = { vert.x, viewport.w - vert.y };
    polygonDepth += vert.z;
    if ( vert.z < 0.0f || vert.z > 1.0f )
      return;
//    std::cout << "Vert " << i << ": " << mVerts[i].x << " " << mVerts[i].y << " " << mVerts[i].z << "\n";
//    std::cout << "Vert " << i << ": " << mVertsProjected[i].x << " " << mVertsProjected[i].y << " " << vert.z << "\n";
//    std::cout << "\n";
  }

  polygonDepth /= mVertsProjected.size();
  depthBuffer.emplace( polygonDepth, this );

  return;
}

void
Poly3D::draw()
{
//  if ( ( (mVertsProjected[1].x - mVertsProjected[0].x) * (mVertsProjected[2].y - mVertsProjected[1].y) )
//       - ((mVertsProjected[1].y - mVertsProjected[0].y) * (mVertsProjected[2].x-mVertsProjected[1].x) ) > 0.0f )
//    return;

  float sum = {};
  for ( size_t i = 0;
        i < mVertsProjected.size();
        ++i )
  {
    const size_t iNext = (i + 1) % mVertsProjected.size();
    sum += (mVertsProjected[iNext].x - mVertsProjected[i].x)
        * (mVertsProjected[iNext].y + mVertsProjected[i].y);
  }

  if ( sum < 0.0f )
//    return;
  {
    std::cout << mVertsProjected[0].x << " " << mVertsProjected[0].y << "\n";
    std::cout << mVertsProjected[1].x << " " << mVertsProjected[1].y << "\n";
    std::cout << mVertsProjected[2].x << " " << mVertsProjected[2].y << "\n";
    std::cout << mVertsProjected[3].x << " " << mVertsProjected[3].y << "\n\n";
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[0], mVertsProjected[1], mColor );
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[1], mVertsProjected[2], mColor );
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[2], mVertsProjected[3], mColor );
    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[3], mVertsProjected[0], mColor );

//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[3], 5.0f, olc::RED );
//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[1], 5.0f, olc::GREEN );
//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[0], 5.0f, olc::BLUE );

    return;
  }

//  if ( mDecal == nullptr )
//  {
//    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[0], mVertsProjected[1], mColor );
//    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[1], mVertsProjected[2], mColor );
//    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[2], mVertsProjected[3], mColor );
//    olc::renderer->ptrPGE->DrawLineDecal( mVertsProjected[3], mVertsProjected[0], mColor );

//    return;
//  }
//  else
    olc::renderer->ptrPGE->DrawWarpedDecal( mDecal, mVertsProjected );

//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[3], 5.0f, olc::RED );
//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[1], 5.0f, olc::GREEN );
//    olc::renderer->ptrPGE->FillCircle( mVertsProjected[0], 5.0f, olc::BLUE );
}

void
Poly3D::setColor( olc::Pixel color )
{
  mColor = color;
}


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
