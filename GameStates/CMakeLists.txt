target_sources(${TARGET}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/GameStates.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateController.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/GameState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameState.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateForeword.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateForeword.hpp

    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateTitle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameStateTitle.hpp
)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
