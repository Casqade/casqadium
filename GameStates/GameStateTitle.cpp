#include <SFML/Audio/Music.hpp>

#include <GameStateTitle.hpp>
#include <Logger.hpp>
#include <Variables.hpp>

#include <glm/vec3.hpp>

#include <olcPGE/olcPGEX_CustomFont.hpp>

#include <TimeUtils/Duration.hpp>

#include <cmath>
#include <sstream>


void
toCameraCoordinates( const glm::vec3 p )
{
  typedef std::array <std::array <float, 4>, 3> Matrix3x4;

  olc::vf2d cameraOrigin = {};

//  X , Y , Z     -  world axis in camera coords
//  Wx, Wy, Wz, 0
//  Wx, Wy, Wz, 0
//  Wx, Wy, Wz, 0
//  0 , 0 , 0 , 1

//  Cx, Cx, Cx, 0 - Camera X axis in world coords
//  Cy, Cy, Cy, 0 - Camera Y axis in world coords
//  Cz, Cz, Cz, 0 - Camera Z axis in world coords
//  0 , 0 , 0 , 1

//    Rotation matrix R:
//  Rx Ry Rz 0 - Right
//  Ux Uy Uz 0 - Up         vectors
//  Dx Dy Dz 0 - Direction
//  0  0  0  1

//    Translation matrix T:
//  1 0 0 -Px, P - camera position vector
//  0 1 0 -Py
//  0 0 1 -Pz
//  0 0 0  1

// LookAt = R * T

//  Matrix3x4 rotatedCameraMatrix;

//  Matrix3x4 cameraPosMatrix;
//  cameraPosMatrix[0][0] = 1;
//  cameraPosMatrix[1][1] = 1;
//  cameraPosMatrix[2][2] = 1;
//  cameraPosMatrix[3][0] = -cameraPos.x;
//  cameraPosMatrix[3][1] = -cameraPos.y;
//  cameraPosMatrix[3][2] = -cameraPos.z;
//  cameraPosMatrix[3][3] = 1;

//  rotatedCameraMatrix[0][0] = nearZ;
//  rotatedCameraMatrix[1][1] = nearZ;
//  rotatedCameraMatrix[0][2] = cameraOrigin.x;
//  rotatedCameraMatrix[1][2] = cameraOrigin.y;
//  rotatedCameraMatrix[2][2] = 1.0f;
}


extern std::vector <std::string> BackStoryTextEntries;
extern std::vector <std::string> TitleTextEntries;

glm::vec3 cubePos = {};
glm::vec3 cameraPos = {};
glm::vec3 cameraRot = {};
const float nearZ = 2.0f;

GameStateTitle::GameStateTitle( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Title)
  , mTitlePos({0.0f, 0.0f, 0.75f})
  , mBackstoryPos({ 0.0f, 150.0f, 0.0f })
  , mPressedKeys()
{}

void
GameStateTitle::updateTitle( const TimeUtils::Duration dt )
{
  if ( mPressedKeys.size() > 0 )
    mTitlePos.z += 0.5 * (double) dt;

  for ( auto it = TitleTextEntries.begin();
        it != TitleTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - TitleTextEntries.begin();

//    move text away
  }
}

void
GameStateTitle::updateBackStory( const TimeUtils::Duration dt )
{
  if ( mPressedKeys.size() > 0 )
    mBackstoryPos.z += 0.5 * (double) dt;

  for ( auto it = BackStoryTextEntries.begin();
        it != BackStoryTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - BackStoryTextEntries.begin();

//    move text up
  }
}

bool
GameStateTitle::update( const uint32_t ticks,
                        const TimeUtils::Duration tickInterval )
{
  const double dt = (double) tickInterval;

  static auto titleDelay = TimeUtils::Duration(2.0);
  if ( (titleDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateTitle( dt );

  static auto backStoryDelay = TimeUtils::Duration(10.0);
  if ( (backStoryDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateBackStory( dt );

  return true;
}

void
GameStateTitle::keyEvent( const olc::Event event )
{
  const olc::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case olc::Event::EventType::KeyPressed:
      mPressedKeys.emplace( key.code );
      static sf::Music& music = Music::Get(MusicId::TitleTheme);
      if ( music.getStatus() != sf::Music::Playing )
        music.play();
      return;

    case olc::Event::EventType::KeyReleased:
    {
      auto pressedKey = mPressedKeys.find(key.code);
      if ( pressedKey != mPressedKeys.end() )
        mPressedKeys.erase( pressedKey );

      if ( mPressedKeys.size() > 0 )
        return;

      Music::Get(MusicId::TitleTheme).pause();

      return;
    }

    default:
      return;
  }
}

void
GameStateTitle::render()
{
  for ( auto& text : TitleTextEntries )
    mPGE->DrawStringDecal( {}, text );

  for ( auto& text : BackStoryTextEntries )
    mPGE->DrawStringDecal( {}, text );
}
