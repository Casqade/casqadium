#include <GameStateController.hpp>

#include <GameStateEcsSandbox.hpp>
//#include <GameStateSandbox.hpp>
//#include <GameStateForeword.hpp>
//#include <GameStateTitle.hpp>

#include <TimeUtils/Duration.hpp>

#include <cqde/util/logger.hpp>

#include <cxxabi.h>


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

template <class NewState>
void
GameStateController::setState()
{
  LOG_TRACE("Changing state to {}", getTypeName <NewState> ());
  mCurrentState = std::make_unique <NewState> ( this );
}

void
GameStateController::handleEvent( const olc::Event event )
{
  mCurrentState->handleEvent( event );
}

bool
GameStateController::update(
  const uint32_t ticks,
  const TimeUtils::Duration dt )
{
  return mCurrentState->update( ticks, dt );
}

void
GameStateController::render()
{
  mCurrentState->render();
}

//template
//void
//GameStateController::setState <GameStateForeword> ();

//template
//void
//GameStateController::setState <GameStateTitle> ();

//template
//void
//GameStateController::setState <GameStateSandbox> ();

template
void
GameStateController::setState <GameStateEcsSandbox> ();
