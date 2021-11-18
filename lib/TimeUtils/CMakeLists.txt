cmake_minimum_required(VERSION 3.7)

set(TARGET TimeUtils)
set(${TARGET}_VERSION "1.0.0")
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

option(${TARGET}_BUILD_SHARED "Build shared library" OFF)
option(${TARGET}_TEST "Enable ${TARGET} test features" OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(${TARGET}_BUILD_SHARED)
  add_library(${TARGET} SHARED)
else()
  add_library(${TARGET})
endif()

add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
  src/Duration.cpp
  include/${TARGET}/Duration.hpp

  src/Timer.cpp
  include/${TARGET}/Timer.hpp
)

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
