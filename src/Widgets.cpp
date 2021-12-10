#include <Widgets.hpp>

#include <glm/gtc/matrix_transform.hpp>

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
  const glm::vec3   origin,
  const glm::vec3   orientation )
  : mProjection(projection)
  , mViewport(viewport)
  , mOrigin(origin)
  , mOrientation(orientation)
  , mFront(glm::vec3(0.0f, 0.0f, -1.0f))
  , mUp()
  , mRight()
  , mWorldUp(0.0f, 1.0f, 0.0f)
  , mSpeed(1.0f)
  , mZoom(45.0f)
{}

void
Camera3D::recalculateVectors()
{
  mFront.x = cos(mOrientation.y) * cos(mOrientation.x);
  mFront.y = sin(mOrientation.x);
  mFront.z = sin(mOrientation.y) * cos(mOrientation.x);
  mFront = glm::normalize(mFront);

  mRight = glm::normalize(glm::cross(mFront, mWorldUp));
  mUp    = glm::normalize(glm::cross(mRight, mFront));
}

glm::mat4
Camera3D::viewMatrix() const
{
  return glm::lookAt( mOrigin, mOrigin + mFront, mUp );
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
                        const glm::vec3 orientation,
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
        * glm::orientate4( mOrientation )
        * glm::scale( mScale );
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


Poly3D::Poly3D(
  const std::array <glm::vec3, 4>& verts,
  const glm::vec3   origin,
  const glm::vec3   orientation,
  const glm::vec3   scale,
  olc::Decal* decal )
  : Drawable3D(origin, orientation, scale)
  , mNormal(glm::normalize(glm::cross(verts[2] - verts[0], verts[1] - verts[0])))
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
    glm::vec3 vert = glm::projectZO( mVerts[i],
                                     modelView,
                                     projection,
                                      viewport );

    mVertsProjected[i] = { vert.x, vert.y };
    polygonDepth += vert.z;
    std::cout << "VertDepth: " << vert.z << "\n";
  }

  polygonDepth /= mVertsProjected.size();
  depthBuffer.emplace( polygonDepth, this );

  return;
}

void
Poly3D::draw()
{
  olc::renderer->ptrPGE->DrawWarpedDecal( mDecal, mVertsProjected );
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
