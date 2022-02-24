cmake_minimum_required(VERSION 3.8)

set(TARGET Graphics3D)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
  src/Camera.cpp
  include/${TARGET}/Camera.hpp

  src/Transformable.cpp
  include/${TARGET}/Transformable.hpp

  src/SceneNode.cpp
  include/${TARGET}/SceneNode.hpp

  src/Quad.cpp
  include/${TARGET}/Quad.hpp
)

target_link_libraries(${TARGET} PUBLIC glm::glm)
target_link_libraries(${TARGET} PUBLIC olcPGE::olcPGE)

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

