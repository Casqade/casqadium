cmake_minimum_required(VERSION 3.7 FATAL_ERROR)

set(TARGET AnotherDayAtHospital)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

option(${TARGET}_INSTALL "Generate install target for ${TARGET}" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(WIN32)
  option(${TARGET}_ENABLE_CMD "Build as console application" ON)
endif()

add_executable(${TARGET})
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

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
)

add_subdirectory(GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

target_link_libraries(${TARGET} PRIVATE TimeUtils::TimeUtils)
target_link_libraries(${TARGET} PRIVATE Sdl2Wrapper::Sdl2Wrapper)
target_link_libraries(${TARGET} PRIVATE AppFramework::AppFramework)

include(GNUInstallDirs)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

if(WIN32)
  if(${${TARGET}_ENABLE_CMD})
    target_link_libraries(${TARGET} PRIVATE -mconsole)
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

endif()

if(${TARGET}_INSTALL)
  set(${TARGET}_INSTALL_PATH ${CMAKE_INSTALL_PREFIX})
else()
  set(${TARGET}_INSTALL_PATH ${CMAKE_BINARY_DIR})
endif()

install(TARGETS ${TARGET} DESTINATION ${${TARGET}_INSTALL_PATH}/${CMAKE_INSTALL_BINDIR})
file(COPY assets DESTINATION ${${TARGET}_INSTALL_PATH}/${CMAKE_INSTALL_BINDIR})
