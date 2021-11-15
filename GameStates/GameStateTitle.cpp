#include <GameStateTitle.hpp>
#include <Logger.hpp>
#include <Variables.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio/Music.hpp>

#include <cmath>


extern std::vector <sf::Text> BackStoryTextEntries;
extern std::vector <sf::Text> TitleTextEntries;


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
    mBackstoryPos.x -= textSpeed * dt;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::D ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::A ) == false )
    mBackstoryPos.x += textSpeed * dt;

  if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::W ) &&
       sf::Keyboard::isKeyPressed( sf::Keyboard::Key::S ) == false )
    mTitlePos.y -= textSpeed * dt;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::S ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::W ) == false )
    mTitlePos.y += textSpeed * dt;

  if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Up ) &&
       sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Down ) == false )
    mTitlePos.z += dt * 0.5f;
  else if ( sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Down ) &&
            sf::Keyboard::isKeyPressed( sf::Keyboard::Key::Up ) == false )
    mTitlePos.z -= dt * 0.5f;
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

  handleControls( dt );

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
}
