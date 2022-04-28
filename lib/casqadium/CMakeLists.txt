cmake_minimum_required(VERSION 3.8)

set(TARGET casqadium)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

set(CASQADIUM_SHORT cqde)

target_sources(${TARGET} PRIVATE
  src/common.cpp
  include/${CASQADIUM_SHORT}/common.hpp

  include/${CASQADIUM_SHORT}/alias.hpp

  src/PackageLoader.cpp
  include/${CASQADIUM_SHORT}/PackageLoader.hpp

  src/types/CallbackStorage.cpp
  include/${CASQADIUM_SHORT}/types/CallbackStorage.hpp

  src/types/InputCallbackStorage.cpp
  include/${CASQADIUM_SHORT}/types/InputCallbackStorage.hpp

  src/types/EntityReference.cpp
  include/${CASQADIUM_SHORT}/types/EntityReference.hpp

  src/types/EntityTagStorage.cpp
  include/${CASQADIUM_SHORT}/types/EntityTagStorage.hpp

  src/types/InputAxis.cpp
  include/${CASQADIUM_SHORT}/types/InputAxis.hpp

  include/${CASQADIUM_SHORT}/types/SequenceStep.hpp

  src/types/VertexBuffer.cpp
  include/${CASQADIUM_SHORT}/types/VertexBuffer.hpp


  src/components/Camera.cpp
  include/${CASQADIUM_SHORT}/components/Camera.hpp

  src/components/SceneNode.cpp
  include/${CASQADIUM_SHORT}/components/SceneNode.hpp

  src/components/SequenceManager.cpp
  include/${CASQADIUM_SHORT}/components/SequenceManager.hpp

  src/components/Transform.cpp
  include/${CASQADIUM_SHORT}/components/Transform.hpp

  src/components/Tag.cpp
  include/${CASQADIUM_SHORT}/components/Tag.hpp

  src/components/GeometryBuffer.cpp
  include/${CASQADIUM_SHORT}/components/GeometryBuffer.hpp

  src/components/InputController.cpp
  include/${CASQADIUM_SHORT}/components/InputController.hpp

  src/components/TextureBuffer.cpp
  include/${CASQADIUM_SHORT}/components/TextureBuffer.hpp


  src/systems/CullingSystem.cpp
  include/${CASQADIUM_SHORT}/systems/CullingSystem.hpp

  src/systems/RenderSystem.cpp
  include/${CASQADIUM_SHORT}/systems/RenderSystem.hpp
)

find_package(EnTT QUIET)

if(NOT EnTT_FOUND)
  include(FetchContent)

  FetchContent_Declare(EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt
    GIT_TAG        v3.10.1
    GIT_SHALLOW    TRUE
  )

  if(${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(EnTT)

endif()

find_package(glm REQUIRED)

find_package(JsonCPP QUIET)

if(NOT JsonCPP_FOUND)
  include(FetchContent)

  FetchContent_Declare(JsonCPP
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
    GIT_TAG        1.9.5
    GIT_SHALLOW    TRUE
  )

  set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
  set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
  set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "" FORCE)

  if(${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
    set(BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE)
  else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(JSONCPP_STATIC_WINDOWS_RUNTIME ON CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(JsonCPP)

endif()

target_link_libraries(${TARGET} PUBLIC
  EnTT::EnTT
  glm
  olcPGE::olcPGE
)

if(${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC jsoncpp_lib)
else()
  target_link_libraries(${TARGET} PUBLIC jsoncpp_static)
endif()

include(GNUInstallDirs)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
