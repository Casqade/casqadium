#include <SFML/Graphics.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Audio.hpp>

#include <AppFramework/ApplicationCore.hpp>
#include <TimeUtils/Duration.hpp>

#include <Variables.hpp>
#include <Logger.hpp>
#include <GameStates.hpp>
#include <GameStateController.hpp>
#include <AppFramework/RenderCommand.hpp>
#include <AppFramework/RenderQueue.hpp>

#include <forward_list>
#include <sstream>
#include <cmath>
#include <memory>
#include <functional>

Logger Log;

sf::RenderWindow window;


std::vector <sf::Text> TitleTextEntries;
std::vector <sf::Text> BackStoryTextEntries;

sf::String backStoryText {"Episode IV;\
A NEW HOPE;\
It is a period of civil war.;\
Rebel spaceships, striking;\
from a hidden base, have won;\
their first victory against;\
the evil Galactic Empire.;\
;\
During the battle, Rebel;\
spies managed to steal secret;\
plans to the Empire's;\
ultimate weapon, the DEATH;\
STAR, an armored space;\
station with enough power to;\
destroy an entire planet.;\
;\
Pursued by the Empire's;\
sinister agents, Princess;\
Leia races home aboard her;\
starship, custodian of the;\
stolen plans that can save;\
her people and restore;\
freedom to the galaxy...."};




int
loop( const uint32_t ticks,
      const TimeUtils::Duration& deltaTime )
{
  static sf::Clock clock;

  sf::Event event;
  while ( window.pollEvent( event ) )
  {
    switch (event.type)
    {
      case sf::Event::Closed:
        window.close();
        return 0;

      default:
        GameStateController::handleEvent( event );
    }
  }

  RenderQueue::Current().push( std::make_shared <RenderCommand> ( [=] ( const TimeUtils::Duration )
  {
    window.setActive();
  } ) );

  GameStateController::update( clock.restart() );
  GameStateController::render( window );

  RenderQueue::Current().push( std::make_shared <RenderCommand> ( [=] ( const TimeUtils::Duration )
  {
    window.display();
    window.clear();
  } ) );

  return -1;
}


int
main( int , char*[] )
{
  sf::err().rdbuf( Log.rdbuf());

  std::forward_list <std::pair <StringId, sf::String>> strings =
  {
    { StringId::Foreword, "A long time ago in a galaxy far,\nfar away..." },
    { StringId::Title, "Another Day;At Hospital" },
    { StringId::Backstory, backStoryText },
    { StringId::ForewordInputPrompt, "Hold any key" },
  };

  std::forward_list <std::pair <FontId, std::string>> fonts =
  {
    { FontId::Munro, "munro.ttf" },
    { FontId::Jetbrains, "jetbrains.ttf" },
    { FontId::FranklinGothic, "framd.ttf" },
    { FontId::FranklinGothicItalic, "framdit.ttf" },
  };

  std::forward_list <std::pair <TextureId, std::string>> textures =
  {
    { TextureId::WindowIcon, "window_icon.png" },
  };

  std::forward_list <std::pair <SoundId, std::string>> sounds =
  {
    { SoundId::Null, "" },
  };

  std::forward_list <std::pair <MusicId, std::string>> musics =
  {
    { MusicId::TitleTheme, "title.ogg" },
  };

  for ( const auto& string : strings )
    if ( Strings::Load( string.first, string.second ) == false )
      return 1;

  for ( const auto& font : fonts )
    if ( Fonts::Load( font.first, font.second ) == false )
      return 1;

  for ( const auto& texture : textures )
    if ( Textures::Load( texture.first, texture.second ) == false )
      return 1;

  for ( const auto& sound : sounds )
    if ( Sounds::Load( sound.first, sound.second ) == false )
      return 1;

  for ( const auto& music : musics )
    if ( Music::Load( music.first, music.second ) == false )
      return 1;

  const auto desktop = sf::VideoMode::getDesktopMode();
  window.create( sf::VideoMode(800, 600),
                 "Another Day At Hospital",
                 sf::Style::Titlebar | sf::Style::Close );
window.setActive(false);
  std::string entry {};
  std::vector <std::string> lines {};

  std::istringstream titleText ( Strings::Get( StringId::Title ));
  while ( std::getline( titleText, entry, ';' ) )
    lines.push_back( entry );

  for ( auto& line : lines )
  {
    sf::Text text( line, Fonts::Get(FontId::FranklinGothic), 128 );

    text.setOutlineColor( sf::Color::Transparent );
    text.setFillColor( sf::Color::Transparent );
    text.setOutlineThickness( 1.0f );
    text.setOrigin( text.getLocalBounds().width * 0.5f,
                    text.getLocalBounds().height * 0.5f );

    TitleTextEntries.push_back( text );
  }

  lines.clear();

  std::istringstream backStoryText( Strings::Get( StringId::Backstory ) );
  while ( std::getline( backStoryText, entry, ';' ) )
    lines.push_back( entry );

  for ( auto& line : lines )
  {
    sf::Text text( line, Fonts::Get(FontId::FranklinGothic), 64 );

    text.setFillColor( sf::Color::Transparent );
    text.setOrigin( text.getLocalBounds().width * 0.5f,
                    text.getLocalBounds().height * 0.5f );

    BackStoryTextEntries.push_back( text );
  }

//  window.setMouseCursorVisible( false );

  GameStateController::setState <GameStateTitle> ();

  return ApplicationCore::Run( &loop, 30, 60 );
}
