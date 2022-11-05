#include <GameStateController.hpp>

#include <GameStateDemo.hpp>

#include <cqde/types/ConfigManager.hpp>

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
GameStateController::setState(
  const cqde::types::ConfigManager& configManager )
{
  LOG_TRACE("Changing state to {}", getTypeName <NewState> ());
  mCurrentState = std::make_unique <NewState> ( this, configManager );
}

void
GameStateController::clearState()
{
  mCurrentState.reset();
}

void
GameStateController::handleEvent( const olc::Event event )
{
  mCurrentState->handleEvent( event );
}

bool
GameStateController::update(
  const uint32_t ticks,
  const TimeUtils::Duration& interval )
{
  return mCurrentState->update( ticks, interval );
}

void
GameStateController::render(
  const uint32_t frames,
  const TimeUtils::Duration& interval )
{
  mCurrentState->render(frames, interval);
}

template
void
GameStateController::setState <GameStateDemo> ( const cqde::types::ConfigManager& );
