#include <GameStateTitle.hpp>
#include <Logger.hpp>
#include <Variables.hpp>

#include <olcPGE/olcPGEX_CustomFont.hpp>

#include <SFML/Audio/Music.hpp>

#include <TimeUtils/Duration.hpp>

#include <cmath>
#include <sstream>


template <uint32_t Rows,
          uint32_t Columns,
          typename Type>
struct Matrix
{
  std::array <std::array <Type, Columns>, Rows> m;
};



void
toCameraCoordinates( const olc::Vector3f p )
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

olc::Vector3f cubePos = {};
olc::Vector3f cameraPos = {};
olc::Vector3f cameraRot = {};
const float nearZ = 2.0f;
olc::vu2d windowSize = {};

GameStateTitle::GameStateTitle( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Title)
  , mTitlePos({0.0f, 0.0f, 0.75f})
  , mBackstoryPos({ 0.0f, 150.0f, 0.0f })
  , mLines({})
  , mPressedKeys()
{windowSize = window.getSize();}

void
GameStateTitle::handleControls( const float dt )
{
  if ( mPressedKeys.size() == 0 )
    return  Music::Get(MusicId::TitleTheme).pause();

  const float textSpeed = 1000.0f;

  if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::A ) &&
       sf::Keyboard::isKeyPressed( sf::Keyboard::Key::D ) == false )
    cubePos.x -= textSpeed * dt;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::D ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::A ) == false )
    cubePos.x += textSpeed * dt;

  if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::W ) &&
       sf::Keyboard::isKeyPressed( sf::Keyboard::Key::S ) == false )
    cubePos.y -= textSpeed * dt;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::S ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::W ) == false )
    cubePos.y += textSpeed * dt;

  if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Up ) &&
       sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Down ) == false )
    cubePos.z += textSpeed * dt;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Down ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Up ) == false )
    cubePos.z -= textSpeed * dt;
}


olc::RectF
rectProjection(
  const olc::Vector3f rectPos,
  const olc::vf2d     rectSize )
{
  olc::vf2d rectProjection1
  {
    (rectPos.x - rectSize.x / 2) / rectPos.z,
    (rectPos.y - rectSize.y / 2) / rectPos.z,
  };

  olc::vf2d rectProjection2
  {
    (rectPos.x + rectSize.x / 2) / rectPos.z,
    (rectPos.y + rectSize.y / 2) / rectPos.z
  };

  olc::vf2d rectProjectionL = rectProjection2 - rectProjection1;
  olc::vf2d rectProjectionO = rectProjection1 + rectProjectionL / 2.0f;

  return
  {
    rectProjectionO,
    rectProjectionL
  };
}

olc::vf2d
pointProjection( const olc::Vector3f p,
                 const float d )
{
  return
  {
    (p.x * nearZ) / (p.z + nearZ),
    (p.y * nearZ) / (p.z + nearZ),
  };
}

float
distance( const olc::Vector3f p1,
          const olc::Vector3f p2 )
{
//  return std::sqrt( std::pow(p2.z - p1.z, 2) );
  return std::sqrt( std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2) + std::pow(p2.z - p1.z, 2) );
}

void
GameStateTitle::updateLines( const float dt )
{
  const float edgeL = 250.0f;
  std::array <olc::Vector3f, 8> cube;
  cube[0] = { -edgeL, -edgeL, -edgeL };
  cube[1] = { -edgeL, -edgeL, edgeL };
  cube[2] = { -edgeL, edgeL, -edgeL};
  cube[3] = { -edgeL, edgeL, edgeL};
  cube[4] = { edgeL, -edgeL, -edgeL };
  cube[5] = { edgeL, -edgeL, edgeL };
  cube[6] = { edgeL, edgeL, -edgeL };
  cube[7] = { edgeL, edgeL, edgeL };

  for ( auto& v : cube )
  {
    v += cubePos;
    std::stringstream coords;
    coords << v.x << ", " << v.y << ", " << v.z << " x ";
    auto proj = pointProjection( v, distance( {}, v ) );
    v.x = windowSize.x * 0.5f + proj.x * edgeL;
    v.y =  windowSize.y * 0.5f + proj.y * edgeL;
    coords << cube[7].x << ", " << cube[7].y << ", " << cube[7].z;
//    Log << coords.str();
  };

  mLines.clear();
  mLines.push_back({cube[0].x, cube[0].y});
  mLines.push_back({cube[1].x, cube[1].y});
  mLines.push_back({cube[1].x, cube[1].y});
  mLines.push_back({cube[3].x, cube[3].y});
  mLines.push_back({cube[3].x, cube[3].y});
  mLines.push_back({cube[2].x, cube[2].y});
  mLines.push_back({cube[2].x, cube[2].y});
  mLines.push_back({cube[0].x, cube[0].y});
  mLines.push_back({cube[0].x, cube[0].y});
  mLines.push_back({cube[4].x, cube[4].y});
  mLines.push_back({cube[4].x, cube[4].y});
  mLines.push_back({cube[6].x, cube[6].y});
  mLines.push_back({cube[6].x, cube[6].y});
  mLines.push_back({cube[2].x, cube[2].y});

  mLines.push_back({cube[1].x, cube[1].y});
  mLines.push_back({cube[5].x, cube[5].y});
  mLines.push_back({cube[5].x, cube[5].y});
  mLines.push_back({cube[4].x, cube[4].y});
  mLines.push_back({cube[5].x, cube[5].y});
  mLines.push_back({cube[7].x, cube[7].y});

  mLines.push_back({cube[3].x, cube[3].y});
  mLines.push_back({cube[7].x, cube[7].y});
  mLines.push_back({cube[7].x, cube[7].y});
  mLines.push_back({cube[6].x, cube[6].y});

//  for ( int i = 0;
//        i < cube.size();
//        ++i)
//  {
//    mLines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 1) % cube.size()].x, cube[(i + 1) % cube.size()].y }});
//    mLines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 2) % cube.size()].x, cube[(i + 2) % cube.size()].y }});
//    mLines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 3) % cube.size()].x, cube[(i + 3) % cube.size()].y }});
//  }

  for ( int i = 0;
        i < mLines.size();
        ++i)
  {
      std::stringstream coords;
      coords << "[" << mLines[i].x << " x " << mLines[i].y << "]";
      static std::map <int, std::string> coordsPrev;

//      if ( coords.str() != coordsPrev[i] )
//        Log << coords.str();

      coordsPrev[i] = coords.str();
  }
}


void
GameStateTitle::updateTitle( const float dt )
{
  if ( mPressedKeys.size() > 0 )
    mTitlePos.z += 0.5f * dt;

  for ( auto it = TitleTextEntries.begin();
        it != TitleTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - TitleTextEntries.begin();
    const sf::Vector3f linePos = { mTitlePos.x,
                                   mTitlePos.y + (lineIndex - 0.5f) * (line.getLocalBounds().top + line.getLocalBounds().height) * 1.25f,
                                   mTitlePos.z };

    line.setOrigin( (line.getLocalBounds().left + line.getLocalBounds().width) * 0.5f,
                    (line.getLocalBounds().top + line.getLocalBounds().height) * 0.5f);

    const auto titleProjection = rectProjection(  linePos,
                                                  line.getLocalBounds() );

    line.setOutlineColor( sf::Color::Yellow );

    line.setPosition( windowSize.x * 0.5f + titleProjection.left,
                      windowSize.y * 0.5f + titleProjection.top );

    line.setScale(  titleProjection.width / line.getLocalBounds().width,
                    titleProjection.height / line.getLocalBounds().height );
  }
}

void
GameStateTitle::updateBackStory( const float dt )
{
  if ( mPressedKeys.size() > 0 )
    mBackstoryPos.z += 0.5f * dt;

  for ( auto it = BackStoryTextEntries.begin();
        it != BackStoryTextEntries.end();
        ++it )
  {
    auto& line = *it;
    const auto lineIndex = it - BackStoryTextEntries.begin();
    const olc::Vector3f linePos = { mBackstoryPos.x,
                                    mBackstoryPos.y,
                                    mBackstoryPos.z - lineIndex * 0.5f };

    const auto lineBounds = Fonts::Get( FontId::Jetbrains).GetTextSize(line);
    const auto textProjection = rectProjection( linePos,
                                                lineBounds );

    if ( linePos.z < 0.0f )
      return;

    line.setFillColor( sf::Color::Yellow );

    line.setPosition( windowSize.x * 0.5f + textProjection.left,
                      windowSize.y * 0.5f + textProjection.top );

    line.setScale( textProjection.width / lineBounds.x,
                   textProjection.height / lineBounds.y );
  }
}

bool
GameStateTitle::update( const uint32_t ticks,
                        const TimeUtils::Duration tickInterval )
{
  const float dt = (double) tickInterval;
  updateLines(dt);

  handleControls( dt );
return true;

  static auto titleDelay = TimeUtils::Duration(2.0);
  if ( (titleDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateTitle( dt );

  static auto backStoryDelay = TimeUtils::Duration(10.0);
  if ( (backStoryDelay -= tickInterval) > TimeUtils::Duration() )
    return true;

  updateBackStory( dt );
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
      if ( music.getStatus() != sf::Music::Status::Playing )
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
  for ( auto text : TitleTextEntries )
    window.draw( text );

  for ( auto text : BackStoryTextEntries )
    window.draw( text );

  window.draw( mLines );
}
