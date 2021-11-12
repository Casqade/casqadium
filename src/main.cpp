#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Err.hpp>

#include <Assets.hpp>
#include <Logger.hpp>
#include <GameState.hpp>

#include <forward_list>
#include <sstream>


Logger Log;

Assets <sf::Font> Fonts;
Assets <sf::Texture> Textures;


std::vector <sf::Text> IntroTextEntries;

std::istringstream introText ("my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;my text goes here;");


template <class Asset>
std::string Assets <Asset>::AssetsPath = "assets/";

template <>
std::string Assets <sf::Font>::AssetType = "fonts/";

template <>
std::string Assets <sf::Texture>::AssetType = "textures/";


class GameStateTitle : public GameState
{
public:
//  void enter() override;
  void update( const sf::Time ) override;
  void render( sf::RenderWindow& ) override;
};

void
GameStateTitle::update( const sf::Time elapsed )
{
  const float dt = elapsed.asSeconds();
  const float textSpeedY = 20.0f * dt;
  const float textScaleD = 1.0f - dt;

  for ( auto& text : IntroTextEntries )
  {
    if ( text.getPosition().y > 0.0f )
      text.move( 0.0f, -textSpeedY );

    if ( text.getScale().x > 0.0f )
      text.scale( textScaleD, textScaleD * 1.0025f );
  }
}

void
GameStateTitle::render( sf::RenderWindow& window )
{
  for ( auto& text : IntroTextEntries )
    window.draw( text );
}


int
main( int , char*[] )
{
  sf::err().rdbuf( Log.rdbuf());

  std::forward_list <std::pair <std::string, std::string>> textures =
  {
    { "window_icon", "window_icon.png" },
  };

  std::forward_list <std::pair <std::string, std::string>> fonts =
  {
    { "munro", "munro.ttf" },
    { "jetbrains", "jetbrains.ttf" },
//    { "framd", "framd.ttf" },
//    { "framdit", "framdit.ttf" },
  };

  for ( const auto& texture : textures )
    if ( Textures.Load( texture.first, texture.second ) == false )
      return 1;

  for ( const auto& font : fonts )
    if ( Fonts.Load( font.first, font.second ) == false )
      return 1;

  sf::RenderWindow window( sf::VideoMode( 800, 600 ), "Another Day At Hospital", sf::Style::Titlebar | sf::Style::Close );

  std::string line {};
  std::vector <std::string> lines {};
  while ( std::getline( introText, line, ';' ) )
    lines.push_back( line );

  for ( auto& str : lines )
  {
    sf::Text text( str, Fonts.Get( "munro" ), 36 );
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

  while ( window.isOpen() )
  {
    static sf::Clock clock;
    static std::unique_ptr <GameState> gameState = std::make_unique <GameStateTitle> ();

    sf::Event event;
    while ( window.pollEvent(event ) )
    {
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close();
          break;

        default:
          gameState->handleEvent( event );
      }
    }

    window.clear();
    gameState->update( clock.restart() );
    gameState->render( window );
    window.display();
  }

  return 0;
}
