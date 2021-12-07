#include <Widgets.hpp>


namespace olc
{

Text2D::Text2D(
  const olc::CustomFont* font,
  const std::string& text,
  const olc::Pixel color )
  : mFont(font)
  , mText(text)
  , mColor(color)
  , mPos()
  , mSize(font->GetTextSize(text))
  , mRotation()
  , mMonospaced(true)
{}

void
Text2D::render() const
{
  if ( mText.empty() )
    return;

  if ( mMonospaced )
    return olc::DrawRotatedFontDecal( mPos, mFont, mText,
                                      mRotation, origin(),
                                      mColor, scale() );

  olc::DrawRotatedFontPropDecal( mPos, mFont, mText,
                                 mRotation, origin(),
                                 mColor, scale() );
}

void
Text2D::setFont( const CustomFont* font )
{
  mFont = font;
}

void
Text2D::setText( const std::string& text )
{
  mText = text;
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
  mPos = origin - mSize;
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
  return mText.empty() ? vf2d(1.0f, 1.0f) : mSize / mFont->GetTextSize(mText);
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

InputPrompt::InputPrompt(
  const std::string& prompt,
  const olc::CustomFont* font,
  const TimeUtils::Duration durationShown,
  const TimeUtils::Duration durationHidden )
  : olc::Text2D(font, prompt)
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
  const olc::vf2d               size,
  const std::pair < olc::Pixel,
                    olc::Pixel> fadeRange,
  const TimeUtils::Duration     fadeDuration,
  const olc::vf2d               pos,
  const olc::vf2d               origin )
  : mTimer(fadeDuration)
  , mFadeRange(fadeRange)
{
//  setFillColor(mFadeRange.first);
//  setOrigin(origin);
//  setPosition(pos);
}

void
FadeEffect::update( const TimeUtils::Duration dt )
{
  if ( mTimer.isReady() )
    return;

  olc::Pixel fadeColor = {}; // getFillColor();

  const auto coef =
    std::min( double(mTimer.duration() - mTimer.update(dt))
                / (double) mTimer.duration(),
              1.0 );

  fadeColor.r
    = mFadeRange.first.r
    + (float(mFadeRange.second.r) - mFadeRange.first.r)
    * coef;

  fadeColor.g
    = mFadeRange.first.g
    + (float(mFadeRange.second.g) - mFadeRange.first.g)
    * coef;

  fadeColor.b
    = mFadeRange.first.b
    + (float(mFadeRange.second.b) - mFadeRange.first.b)
    * coef;

  fadeColor.a
    = mFadeRange.first.a
    + (float(mFadeRange.second.a) - mFadeRange.first.a)
    * coef;

//  setFillColor( fadeColor );
}

void
FadeEffect::setFadeRange(
  std::pair <olc::Pixel, olc::Pixel> fadeRange )
{
  mTimer = { mTimer.duration() };
  mFadeRange = fadeRange;
}

void
FadeEffect::setFadeDuration(
  TimeUtils::Duration fadeDuration )
{
  mTimer = { fadeDuration };
}

bool
FadeEffect::finished() const
{
  return mTimer.isReady();
}
