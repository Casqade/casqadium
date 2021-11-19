cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(TARGET AnotherDayAtHospital)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

if(WIN32)
  option(${TARGET}_DISABLE_CMD "Build as GUI application" OFF)
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(${TARGET}_INSTALL "Generate install target for ${TARGET}" OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT ${BUILD_SHARED_LIBS})
  set(SDL_STATIC_LIBRARIES TRUE)
endif()

add_executable(${TARGET})
set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
)

target_sources(${TARGET} PRIVATE

  include/Variables.hpp

  include/Assets.hpp
  include/Logger.hpp
  include/Widgets.hpp

  src/Assets.cpp
  src/Widgets.cpp
  src/Logger.cpp

  src/main.cpp
  src/icon.rc
)

add_subdirectory(GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/GameStates)

target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)
target_link_libraries(${TARGET} PRIVATE TimeUtils::TimeUtils)
target_link_libraries(${TARGET} PRIVATE AppFramework::AppFramework)
target_link_libraries(${TARGET} PRIVATE Sdl2Wrapper::Sdl2Wrapper)

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
endif()

if(WIN32 AND ${TARGET}_DISABLE_CMD)
  target_link_libraries(${TARGET} mwindows)
endif()

if(${TARGET}_INSTALL)
  set(${TARGET}_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
else()
  set(${TARGET}_INSTALL_PATH ${CMAKE_BINARY_DIR})
endif()

install(TARGETS ${TARGET} DESTINATION ${${TARGET}_INSTALL_PATH}/${CMAKE_INSTALL_BINDIR})
file(COPY assets DESTINATION ${${TARGET}_INSTALL_PATH}/${CMAKE_INSTALL_BINDIR})
