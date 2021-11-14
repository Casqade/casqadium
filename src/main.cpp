#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/Audio.hpp>

#include <Variables.hpp>
#include <Logger.hpp>
#include <GameStates.hpp>
#include <GameStateController.hpp>

#include <forward_list>
#include <sstream>
#include <cmath>
#include <memory>


Logger Log;

sf::RenderWindow window;


std::vector <sf::Text> IntroTextEntries;

std::istringstream introText ("my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;");


int
main( int , char*[] )
{
  sf::err().rdbuf( Log.rdbuf());

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

  window.create( sf::VideoMode( 800, 600 ), "Another Day At Hospital", sf::Style::Titlebar | sf::Style::Close );

  std::string line {};
  std::vector <std::string> lines {};
  while ( std::getline( introText, line, ';' ) )
    lines.push_back( line );

  for ( auto& str : lines )
  {
    sf::Text text( str, Fonts::Get(FontId::Munro), 36 );
    text.setFillColor( sf::Color::Yellow );
    text.setOrigin( text.getLocalBounds().width * 0.5f,
                    text.getLocalBounds().height * 0.5f );
    text.setPosition( window.getSize().x * 0.5f, window.getSize().y * 0.5f + IntroTextEntries.size() * ( text.getLocalBounds().height + text.getLineSpacing() ) );
    text.setScale( 2.0f + IntroTextEntries.size(), 2.0f + IntroTextEntries.size());
    IntroTextEntries.push_back( text );
  }


  window.setFramerateLimit( 60 );
//  window.setVerticalSyncEnabled( true );
//  window.setMouseCursorVisible( false );

  GameStateController::setState <GameStateForeword> ();

  while ( window.isOpen() )
  {
    static sf::Clock clock;

    sf::Event event;
    while ( window.pollEvent(event ) )
    {
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close();
          break;

        default:
          GameStateController::handleEvent( event );
      }
    }

    window.clear();
    GameStateController::update( clock.restart() );
    GameStateController::render( window );
    window.display();
  }

  return 0;
}
