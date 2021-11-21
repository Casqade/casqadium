cmake_minimum_required(VERSION 3.7)

set(TARGET Sdl2Wrapper)
set(${TARGET}_VERSION 0.0.1)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

option(SDL_PNG_SUPPORT "Enable SDL2_image support" OFF)
option(SDL_MIX_SUPPORT "Enable SDL2_mixer support" OFF)
option(SDL_TTF_SUPPORT "Enable SDL2_ttf support" OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
#  src/SDL2.cpp
  src/Window.cpp
  src/Renderer.cpp
  src/Math.cpp
#  src/Texture.cpp
#  src/Animation.cpp
  include/${TARGET}/SDL2.hpp
  include/${TARGET}/Window.hpp
  include/${TARGET}/Renderer.hpp
  include/${TARGET}/Math.hpp
#  include/${TARGET}/Texture.hpp
#  include/${TARGET}/Animation.hpp
)

#target_link_libraries(${TARGET} PRIVATE Logger::Logger)
target_link_libraries(${TARGET} PRIVATE TimeUtils::TimeUtils)

#list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
find_package(SDL2 REQUIRED)

if(${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC SDL2::SDL2)
else()
  target_link_libraries(${TARGET} PUBLIC SDL2::SDL2-static)
endif()

if (SDL_PNG_SUPPORT)
  find_package(SDL2_image REQUIRED)

  target_compile_definitions(${TARGET} PUBLIC SDL_PNG_SUPPORT)
  target_link_libraries(${TARGET} PUBLIC SDL2::Image)
endif(SDL_PNG_SUPPORT)

if (SDL_MIX_SUPPORT)
  target_sources(${TARGET} PRIVATE
    src/Sound.cpp
    include/${TARGET}/Sound.hpp
  )

  find_package(SDL2_mixer REQUIRED)

  target_compile_definitions(${TARGET} PUBLIC SDL_MIX_SUPPORT)
  target_link_libraries(${TARGET} PUBLIC SDL2::Mixer)
endif(SDL_MIX_SUPPORT)

if (SDL_TTF_SUPPORT)
  find_package(SDL2_ttf REQUIRED)

  target_compile_definitions(${TARGET} PUBLIC SDL_TTF_SUPPORT)
  target_link_libraries(${TARGET} PUBLIC SDL2::TTF)
endif(SDL_TTF_SUPPORT)

include(GNUInstallDirs)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
