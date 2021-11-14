#include <GameStateController.hpp>

#include <GameStateForeword.hpp>
#include <GameStateTitle.hpp>

#include <Logger.hpp>
#include <Variables.hpp>

#include <cxxabi.h>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>


template <typename Type>
std::string getTypeName()
{
  std::string typeName = typeid(Type).name();
  char* demangledName = abi::__cxa_demangle(typeName.c_str(), NULL, NULL, NULL);
  if ( demangledName != nullptr )
  {
    typeName = demangledName;
    std::free(demangledName);
  }

  return typeName;
}


std::unique_ptr <GameState>
GameStateController::mCurrentState = {};

template <class NewState>
void
GameStateController::setState()
{
  Log << "Changing state to "_s + getTypeName <NewState> ();
  mCurrentState = std::make_unique <NewState> ();
}

void
GameStateController::handleEvent( const sf::Event event )
{
  mCurrentState->handleEvent( event );
}

void
GameStateController::update( const sf::Time elapsed )
{
  mCurrentState->update( elapsed );
}

void
GameStateController::render( sf::RenderWindow& window )
{
  mCurrentState->render( window );
}

template
void
GameStateController::setState <GameStateForeword> ();

template
void
GameStateController::setState <GameStateTitle> ();
