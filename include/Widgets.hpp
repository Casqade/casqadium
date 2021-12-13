#ifndef WIDGETS_HPP
#define WIDGETS_HPP

#include <olcPGE/olcPGEX_CustomFont.hpp>
#include <olcPGE/olcPixelGameEngine.hpp>

#include <TimeUtils/Duration.hpp>
#include <TimeUtils/Timer.hpp>

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

}

class Camera3D
{
  glm::mat4 mProjection;
  glm::vec4 mViewport;

  glm::vec3 mOrigin;
  glm::quat mOrientation;

  float mSpeed;
  float mZoom;

public:
  Camera3D( const glm::mat4&  projection,
            const glm::vec4&  viewport,
            const glm::vec3&  origin = {},
            const glm::quat&  orientation = glm::vec3{} );

  void move( const glm::vec3& direction );
  void rotate( const glm::quat& );
  void rotateGlobal( const glm::quat& );
//  void zoom(  );

  glm::vec3 origin() const;
  glm::quat orientation() const;
  glm::vec3 front() const;
  glm::vec3 right() const;
  glm::vec3 up() const;

  glm::mat4 viewMatrix() const;
  glm::mat4 projMatrix() const;
  glm::vec4 viewport() const;
};

class Drawable3D
{
protected:
  glm::vec3 mOrigin;
  glm::quat mOrientation;
  glm::vec3 mScale;

  glm::mat4 modelMatrix() const;

public:
  Drawable3D( const glm::vec3 origin = {},
              const glm::quat orientation = glm::vec3{},
              const glm::vec3 scale = { 1.0f, 1.0f, 1.0f } );

  virtual void appendCulled(  std::multimap < float, Drawable3D*, std::greater <float>>& depthBuffer,
                              const Camera3D& );

  virtual void draw();

  void translate( const glm::vec3 );
  void rotate( const glm::quat );
  void scale( const glm::vec3 );

  void setOrigin( const glm::vec3 );
  void setOrientation( const glm::quat );
  void setScale( const glm::vec3 );
};

class Poly3D : public Drawable3D
{
  std::array <glm::vec3, 4> mVerts;
  std::array <olc::vf2d, 4> mVertsProjected;

  olc::Decal* mFrontFaceDecal;
  olc::Decal* mBackFaceDecal;

  olc::Pixel  mFrontFaceColor;
  olc::Pixel  mBackFaceColor;

  enum FaceWindingOrder : bool
  {
    CounterClockWise,
    ClockWise

  } mProjectedWindingOrder;

  bool isClockWise( const bool yAxisUp = false ) const;

public:
  Poly3D( const std::array <glm::vec3, 4>& verts );

  void draw() override;
  void appendCulled(  std::multimap < float, Drawable3D*, std::greater <float>>& depthBuffer,
                      const Camera3D& ) override;

  void setFrontFace( olc::Decal* );
  void setFrontFace( const olc::Pixel );

  void setBackFace( olc::Decal* );
  void setBackFace( const olc::Pixel );
};


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
