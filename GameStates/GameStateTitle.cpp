#include <GameStateTitle.hpp>
#include <Logger.hpp>
#include <Variables.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Audio/Music.hpp>

#include <cmath>
#include <sstream>


extern std::vector <sf::Text> BackStoryTextEntries;
extern std::vector <sf::Text> TitleTextEntries;

sf::Vector3f cubePos = {};
sf::Vector3f cameraPos = {};
sf::Vector3f cameraRot = {};
const float nearZ = 2.0f;

template <uint32_t Rows,
          uint32_t Columns,
          typename Type>
struct Matrix
{
  std::array <std::array <Type, Columns>, Rows> m;
};



void
toCameraCoordinates( const sf::Vector3f p )
{
  typedef std::array <std::array <float, 4>, 3> Matrix3x4;

  sf::Vector2f cameraOrigin = {};

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

  Matrix3x4 rotatedCameraMatrix;

  Matrix3x4 cameraPosMatrix;
  cameraPosMatrix[0][0] = 1;
  cameraPosMatrix[1][1] = 1;
  cameraPosMatrix[2][2] = 1;
  cameraPosMatrix[3][0] = -cameraPos.x;
  cameraPosMatrix[3][1] = -cameraPos.y;
  cameraPosMatrix[3][2] = -cameraPos.z;
  cameraPosMatrix[3][3] = 1;

  rotatedCameraMatrix[0][0] = nearZ;
  rotatedCameraMatrix[1][1] = nearZ;
  rotatedCameraMatrix[0][2] = cameraOrigin.x;
  rotatedCameraMatrix[1][2] = cameraOrigin.y;
  rotatedCameraMatrix[2][2] = 1.0f;
}

GameStateTitle::GameStateTitle()
  : mState(StateLocal::Title)
  , mTitlePos({0.0f, 0.0f, 0.75f})
  , mBackstoryPos({ 0.0f, 150.0f, 0.0f })
  , mPressedKeys()
{}

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


sf::FloatRect
rectProjection(
  const sf::Vector3f  rectPos,
  const sf::FloatRect rect )
{
  sf::Vector2f rectSize
  {
    rect.left + rect.width,
    rect.top + rect.height,
  };

  sf::Vector2f rectProjection1
  {
    (rectPos.x - rectSize.x / 2) / rectPos.z,
    (rectPos.y - rectSize.y / 2) / rectPos.z,
  };

  sf::Vector2f rectProjection2
  {
    (rectPos.x + rectSize.x / 2) / rectPos.z,
    (rectPos.y + rectSize.y / 2) / rectPos.z
  };

  sf::Vector2f rectProjectionL = rectProjection2 - rectProjection1;
  sf::Vector2f rectProjectionO = rectProjection1 + rectProjectionL / 2.0f;

  return
  {
    rectProjectionO,
    rectProjectionL
  };
}

//sf::Vector2f operator / ( const sf::Vector2f l,
//                          const sf::Vector2f r )
//{
//  return
//  {
//    l.x / r.x,
//    l.y / r.y
//  };
//}

//sf::Vector2f operator * ( const sf::Vector2f l,
//                          const sf::Vector2f r )
//{
//  return
//  {
//    l.x * r.x,
//    l.y * r.y
//  };
//}

sf::Vector2f
pointProjection( const sf::Vector3f p,
                 const float d )
{
  return
  {
    (p.x * nearZ) / (p.z + nearZ),
    (p.y * nearZ) / (p.z + nearZ),
  };
}

float
distance( const sf::Vector3f p1,
          const sf::Vector3f p2 )
{
//  return std::sqrt( std::pow(p2.z - p1.z, 2) );
  return std::sqrt( std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2) + std::pow(p2.z - p1.z, 2) );
}

sf::VertexArray lines( sf::Lines );

void
GameStateTitle::updateLines( const float dt )
{
  const float edgeL = 250.0f;
  std::array <sf::Vector3f, 8> cube;
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
    v.x = window.getSize().x * 0.5f + proj.x * edgeL;
    v.y =  window.getSize().y * 0.5f + proj.y * edgeL;
    coords << cube[7].x << ", " << cube[7].y << ", " << cube[7].z;
//    Log << coords.str();
  };

  lines.clear();
  lines.append({{cube[0].x, cube[0].y}});
  lines.append({{cube[1].x, cube[1].y}, sf::Color::Blue});
  lines.append({{cube[1].x, cube[1].y}, sf::Color::Blue});
  lines.append({{cube[3].x, cube[3].y}});
  lines.append({{cube[3].x, cube[3].y}});
  lines.append({{cube[2].x, cube[2].y}, sf::Color::Green});
  lines.append({{cube[2].x, cube[2].y}, sf::Color::Green});
  lines.append({{cube[0].x, cube[0].y}});
  lines.append({{cube[0].x, cube[0].y}});
  lines.append({{cube[4].x, cube[4].y}, sf::Color::Red});
  lines.append({{cube[4].x, cube[4].y}, sf::Color::Red});
  lines.append({{cube[6].x, cube[6].y}});
  lines.append({{cube[6].x, cube[6].y}});
  lines.append({{cube[2].x, cube[2].y}, sf::Color::Green});

  lines.append({{cube[1].x, cube[1].y}, sf::Color::Blue});
  lines.append({{cube[5].x, cube[5].y}});
  lines.append({{cube[5].x, cube[5].y}});
  lines.append({{cube[4].x, cube[4].y}, sf::Color::Red});
  lines.append({{cube[5].x, cube[5].y}});
  lines.append({{cube[7].x, cube[7].y}});

  lines.append({{cube[3].x, cube[3].y}});
  lines.append({{cube[7].x, cube[7].y}});
  lines.append({{cube[7].x, cube[7].y}});
  lines.append({{cube[6].x, cube[6].y}});

//  for ( int i = 0;
//        i < cube.size();
//        ++i)
//  {
//    lines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 1) % cube.size()].x, cube[(i + 1) % cube.size()].y }});
//    lines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 2) % cube.size()].x, cube[(i + 2) % cube.size()].y }});
//    lines.append({{ cube[i].x, cube[i].y },
//                       { cube[(i + 3) % cube.size()].x, cube[(i + 3) % cube.size()].y }});
//  }

  for ( int i = 0;
        i < lines.getVertexCount();
        ++i)
  {
      std::stringstream coords;
      coords << "[" << lines[i].position.x << " x " << lines[i].position.y << "]";
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

    line.setPosition( window.getSize().x * 0.5f + titleProjection.left,
                      window.getSize().y * 0.5f + titleProjection.top );

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
    const sf::Vector3f linePos = { mBackstoryPos.x,
                                   mBackstoryPos.y,
                                   mBackstoryPos.z - lineIndex * 0.5f };

    const auto textProjection = rectProjection( linePos,
                                                line.getLocalBounds() );

    if ( linePos.z < 0.0f )
      return;

    line.setFillColor( sf::Color::Yellow );

    line.setPosition( window.getSize().x * 0.5f + textProjection.left,
                      window.getSize().y * 0.5f + textProjection.top );

    line.setScale( textProjection.width / line.getLocalBounds().width,
                   textProjection.height / line.getLocalBounds().height );
  }
}

void
GameStateTitle::update( const sf::Time elapsed )
{
  const float dt = elapsed.asSeconds();
  updateLines(dt);

  handleControls( dt );
return;
  static float titleDelay = sf::seconds(2.0f).asSeconds();
  if ( (titleDelay -= dt) > 0.0f )
    return;

  updateTitle( dt );

  static float backStoryDelay = sf::seconds(10.0f).asSeconds();
  if ( (backStoryDelay -= dt) > 0.0f )
    return;

  updateBackStory( dt );
}

void
GameStateTitle::keyEvent( const sf::Event event )
{
  const sf::Event::KeyEvent key = event.key;

  switch (event.type)
  {
    case sf::Event::EventType::KeyPressed:
      mPressedKeys.emplace( key.code );
      static sf::Music& music = Music::Get(MusicId::TitleTheme);
      if ( music.getStatus() != sf::Music::Status::Playing )
        music.play();
      return;

    case sf::Event::EventType::KeyReleased:
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
GameStateTitle::render( sf::RenderWindow& window )
{
  for ( auto& text : TitleTextEntries )
    window.draw( text );

  for ( auto& text : BackStoryTextEntries )
    window.draw( text );

  window.draw( lines );
}
