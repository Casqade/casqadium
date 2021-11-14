#include <Widgets.hpp>


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
