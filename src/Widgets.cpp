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
    return mFont->DrawRotatedStringDecal( mPos, mText,
                                          mRotation, origin(),
                                          mColor, scale() );

  mFont->DrawRotatedStringPropDecal( mPos, mText,
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
  const olc::CustomFont& font,
  const TimeUtils::Duration durationShown,
  const TimeUtils::Duration durationHidden )
  : mState(State::Inactive)
  , mDurationShown(durationShown)
  , mDurationHidden(durationHidden)
  , mTimeInState()
  , mFont(font)
{}

void
InputPrompt::update( const TimeUtils::Duration interval )
{
//  switch ( mState )
//  {
//    case State::Inactive:
//    {
//      sf::Color fillColor = this->getFillColor();
//      fillColor.a = 0;
//      return setFillColor( fillColor );
//    }

//    case State::Shown:
//    {
//      if ( mTimeInState.getElapsedTime().asSeconds() > mDurationShown.asSeconds() )
//        return setState( State::Hidden );

//      sf::Color fillColor = this->getFillColor();
//      fillColor.a = 255;

//      return setFillColor( fillColor );
//    }

//    case State::Hidden:
//    {
//      if ( mTimeInState.getElapsedTime().asSeconds() > mDurationHidden.asSeconds() )
//        setState( State::Shown );

//      sf::Color fillColor = this->getFillColor();
//      fillColor.a = 0;

//      return setFillColor( fillColor );
//    }

//    default:
//      return;
//  }
}

void
InputPrompt::setState( const State state )
{
  mState = state;
//  mTimeInState.restart();
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
