#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <olcPGE/olcPGEX_CustomFont.hpp>
#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>
#include <olcPGE/Math.hpp>

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

public:
  Text2D( const olc::CustomFont*,
          const std::string&  = {},
          const olc::Pixel    = olc::WHITE );

  void render() const;

  void setPos( const vf2d );
  void setSize( const vu2d );
  void setOrigin( const vf2d );
  void setRotation( const float );

  void setFont( const olc::CustomFont* );
  void setText( const std::string& );
  void setColor( const olc::Pixel );
  void setMonospaced( const bool );

  olc::CustomFont* font() const;
  std::string text() const;
  olc::Pixel color() const;

  vf2d pos() const;
  vu2d size() const;
  vu2d textSize() const;
  vf2d scale() const;
  vf2d origin() const;
  float rotation() const;
};

class Rect3D
{
  olc::Vector3f mPos;
  olc::Vector3f mRotation;
  olc::Vector3f mNormal;

  std::array <olc::vf2d, 4> mVerts;

  const olc::Decal* mDecal;

public:
  Rect3D( const olc::vf2d     size,
          const olc::Vector3f pos = {},
          const olc::Vector3f rot = {},
          const olc::Decal*   decal = {} );

  void render();

  std::array <olc::Vector3f, 4> verts();
};

olc::vf2d
toScreen( const olc::Vector3f vert )
{

}

bool
isCulled( const Rect3D& )
{
  return true;
}

}

class InputPrompt
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
                const olc::CustomFont& font,
                const TimeUtils::Duration shown,
                const TimeUtils::Duration hidden );

  void update( const TimeUtils::Duration );
  void setState( const State state );

  bool isActive() const;

private:
  State mState;

  TimeUtils::Duration mDurationShown;
  TimeUtils::Duration mDurationHidden;

  TimeUtils::Duration mTimeInState;

  olc::CustomFont& mFont;
};

class FadeEffect
{
  TimeUtils::Duration mFadeDuration;
  float mFadeCurrent;

  std::pair <olc::Pixel, olc::Pixel> mFadeRange;

public:
  FadeEffect(
    const olc::vf2d               size,
    const std::pair < olc::Pixel,
                      olc::Pixel> fadeRange,
    const TimeUtils::Duration     fadeDuration,
    const olc::vf2d               pos = {},
    const olc::vf2d               origin = {} );

  void update( const float dt );

  void setFadeRange( std::pair <olc::Pixel, olc::Pixel> );
  void setFadeDuration( const TimeUtils::Duration );

  bool finished() const;
};


#endif
