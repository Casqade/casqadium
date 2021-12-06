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
  const sf::Text prompt,
  const sf::Time durationShown,
  const sf::Time durationHidden )
  : sf::Text(prompt)
  , mState(State::Inactive)
  , mDurationShown(durationShown)
  , mDurationHidden(durationHidden)
  , mTimeInState()
{}

void
InputPrompt::update( const float dt )
{
  switch ( mState )
  {
    case State::Inactive:
    {
      sf::Color fillColor = this->getFillColor();
      fillColor.a = 0;
      return setFillColor( fillColor );
    }

    case State::Shown:
    {
      if ( mTimeInState.getElapsedTime().asSeconds() > mDurationShown.asSeconds() )
        return setState( State::Hidden );

      sf::Color fillColor = this->getFillColor();
      fillColor.a = 255;

      return setFillColor( fillColor );
    }

    case State::Hidden:
    {
      if ( mTimeInState.getElapsedTime().asSeconds() > mDurationHidden.asSeconds() )
        setState( State::Shown );

      sf::Color fillColor = this->getFillColor();
      fillColor.a = 0;

      return setFillColor( fillColor );
    }

    default:
      return;
  }
}

void
InputPrompt::setState( const State state )
{
  mState = state;
  mTimeInState.restart();
}

bool
InputPrompt::isActive() const
{
  return mState != State::Inactive;
}

FadeEffect::FadeEffect(
  const sf::Vector2f            size,
  const std::pair < sf::Color,
                    sf::Color>  fadeRange,
  const sf::Time                fadeDuration,
  const sf::Vector2f            pos,
  const sf::Vector2f            origin )
  : sf::RectangleShape(size)
  , mFadeDuration(fadeDuration)
  , mFadeCurrent()
  , mFadeRange(fadeRange)
{
  setFillColor(mFadeRange.first);
  setOrigin(origin);
  setPosition(pos);
}

void
FadeEffect::update( const float dt )
{
  if ( mFadeCurrent >= mFadeDuration.asSeconds() )
    return;

  sf::Color fadeColor = getFillColor();

  const auto coef =
    std::min( (mFadeCurrent += dt)
                / mFadeDuration.asSeconds(),
              1.0f );

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

  setFillColor( fadeColor );
}

void
FadeEffect::setFadeRange(
  std::pair < sf::Color, sf::Color> fadeRange )
{
  mFadeCurrent = float();
  mFadeRange = fadeRange;
}

void
FadeEffect::setFadeDuration(
  sf::Time fadeDuration )
{
  mFadeDuration = fadeDuration;
}

bool
FadeEffect::finished() const
{
  return mFadeCurrent >= mFadeDuration.asSeconds();
}
