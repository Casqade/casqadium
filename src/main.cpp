#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/System/Err.hpp>

#include <Assets.hpp>
#include <Logger.hpp>
#include <GameState.hpp>

#include <forward_list>


Logger Log;


class Fonts : public Assets <sf::Font>
{

};

class Textures : public Assets <sf::Texture>
{

};

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

}

void
GameStateTitle::render( sf::RenderWindow& window )
{

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
    if ( Textures::Load( texture.first, texture.second ) == false )
      return 1;

  for ( const auto& font : fonts )
    if ( Textures::Load( font.first, font.second ) == false )
      return 1;

  sf::RenderWindow window;

  std::unique_ptr <GameState> gameState = std::make_unique <GameStateTitle> ();

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

  return 0;
}
