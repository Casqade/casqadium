#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <olcPGE/olcPGEX_CustomFont.hpp>
#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>
#include <TimeUtils/Timer.hpp>
#include <Graphics3D/SceneNode.hpp>

#include <olcPGE/Math.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/quaternion.hpp>

#include <string>


namespace olc
{

class Text2D
{
  const olc::CustomFont* mFont;

  std::string mText;
  olc::Pixel mColor;

  vf2d mPos;
  vu2d mSize;
  float mRotation;

  bool mMonospaced;

  bool mIsHidden;

public:
  Text2D( const olc::CustomFont*,
          const std::string&  = {},
          const olc::Pixel    = olc::WHITE,
          const bool monospaced = true );

  void render() const;

  void setPos( const vf2d );
  void setSize( const vu2d );
  void setOrigin( const vf2d );
  void setRotation( const float );

  void setFont( const olc::CustomFont* );
  void setText( const std::string& );
  void setColor( const olc::Pixel );
  void setMonospaced( const bool );
  void setHidden( const bool );

  const olc::CustomFont* font() const;
  std::string text() const;
  olc::Pixel color() const;

  vf2d pos() const;
  vu2d size() const;
  vu2d textSize() const;
  vf2d scale() const;
  vf2d origin() const;
  float rotation() const;
  bool isHidden() const;
};

}

class InputPrompt : public olc::Text2D
{
public:

  enum class State
  {
      Inactive
    , Shown
    , Hidden

  };

public:
  InputPrompt(  const std::string& prompt,
                const olc::CustomFont* font,
                const TimeUtils::Duration shown,
                const TimeUtils::Duration hidden );

  void update( const TimeUtils::Duration );
  void setState( const State state );

  bool isActive() const;

private:
  State mState;

  TimeUtils::Duration mDurationShown;
  TimeUtils::Duration mDurationHidden;

  TimeUtils::Timer mTimeInState;
};

class FadeEffect : public olc::RectF
{
  olc::Pixel mFadeColor;
  TimeUtils::Timer mFadeTimer;

  std::pair <olc::Pixel, olc::Pixel> mFadeRange;

public:
  FadeEffect(
    const olc::RectF              rect,
    const std::pair < olc::Pixel,
                      olc::Pixel> fadeRange,
    const TimeUtils::Duration     fadeDuration );

  void update( const TimeUtils::Duration );

  void setFadeRange( std::pair <olc::Pixel, olc::Pixel> );
  void setFadeDuration( const TimeUtils::Duration );

  olc::Pixel fadeColor() const;
  bool finished() const;
};


#endif
