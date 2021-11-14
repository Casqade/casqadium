#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>


class InputPrompt : public sf::Text
{
public:

  enum class State
  {
      Inactive
    , Shown
    , Hidden

  };

public:
  InputPrompt(  const sf::Text prompt,
                const sf::Time durationShown,
                const sf::Time durationHidden );

  void update( const float dt );
  void setState( const State state );

  bool isActive() const;

private:
  State mState;

  sf::Time  mDurationShown,
            mDurationHidden;

  sf::Clock mTimeInState;
};

class FadeEffect : public sf::RectangleShape
{
  sf::Time mFadeDuration;
  float mFadeCurrent;

  std::pair <sf::Color, sf::Color> mFadeRange;

public:
  FadeEffect(
    const sf::Vector2f            size,
    const std::pair < sf::Color,
                      sf::Color>  fadeRange,
    const sf::Time                fadeDuration,
    const sf::Vector2f            pos = {},
    const sf::Vector2f            origin = {} );

  void update( const float dt );

  void setFadeRange( std::pair < sf::Color, sf::Color> );
  void setFadeDuration( const sf::Time );

  bool finished() const;
};


#endif
