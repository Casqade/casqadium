#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <olcPGE/olcPGEX_CustomFont.hpp>
#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>
#include <TimeUtils/Timer.hpp>

#include <olcPGE/Math.hpp>
#include <glm/glm.hpp>

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

  const olc::CustomFont* font() const;
  std::string text() const;
  olc::Pixel color() const;

  vf2d pos() const;
  vu2d size() const;
  vu2d textSize() const;
  vf2d scale() const;
  vf2d origin() const;
  float rotation() const;
};


class Camera
{
  glm::vec3 mPos;
  glm::vec3 mDirection;

public:
  Camera( const glm::vec3 up )
  {

  }
};

class Rect3D
{
  glm::vec3 mPos;
  glm::vec3 mRotation;
  glm::vec3 mNormal;

  std::array <olc::vf2d, 4> mVerts;

  const olc::Decal* mDecal;

public:
  Rect3D( const glm::vec2 size,
          const glm::vec3 pos = {},
          const glm::vec3 rot = {},
          const olc::Decal*   decal = {} );

  void render();

  std::array <glm::vec3, 4> verts();
};

inline olc::vf2d
toScreen( const glm::vec3 vert )
{
  return {};
}

bool
inline isCulled( const Rect3D& )
{
  return true;
}

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
