cmake_minimum_required(VERSION 3.8)

set(TARGET ECS)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
  src/Common.cpp
  include/${TARGET}/Common.hpp

  include/${TARGET}/Alias.hpp

  src/PackageLoader.cpp
  include/${TARGET}/PackageLoader.hpp

  src/Types/CallbackStorage.cpp
  include/${TARGET}/Types/CallbackStorage.hpp

  src/Types/EntityReference.cpp
  include/${TARGET}/Types/EntityReference.hpp

  src/Types/InputAxis.cpp
  include/${TARGET}/Types/InputAxis.hpp

  include/${TARGET}/Types/SequenceStep.hpp

  src/Types/VertexBuffer.cpp
  include/${TARGET}/Types/VertexBuffer.hpp


  src/Components/Camera.cpp
  include/${TARGET}/Components/Camera.hpp

  src/Components/SceneNode.cpp
  include/${TARGET}/Components/SceneNode.hpp

  src/Components/SequenceManager.cpp
  include/${TARGET}/Components/SequenceManager.hpp

  src/Components/Transform.cpp
  include/${TARGET}/Components/Transform.hpp

  src/Components/Tag.cpp
  include/${TARGET}/Components/Tag.hpp

  src/Components/GeometryBuffer.cpp
  include/${TARGET}/Components/GeometryBuffer.hpp

  src/Components/InputController.cpp
  include/${TARGET}/Components/InputController.hpp

  src/Components/TextureBuffer.cpp
  include/${TARGET}/Components/TextureBuffer.hpp

  src/Components/TextureStorage.cpp
  include/${TARGET}/Components/TextureStorage.hpp


  src/Systems/CullingSystem.cpp
  include/${TARGET}/Systems/CullingSystem.hpp

  src/Systems/RenderSystem.cpp
  include/${TARGET}/Systems/RenderSystem.hpp
)

find_package(EnTT QUIET)

if(NOT EnTT_FOUND)
  include(FetchContent)

  FetchContent_Declare(EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt
    GIT_TAG        v3.8.0
    GIT_SHALLOW    TRUE
  )

  if(${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(EnTT)

endif()

if(NOT TARGET glm)
  find_package(glm REQUIRED)
endif()

target_link_libraries(${TARGET} PUBLIC
  EnTT::EnTT
  glm::glm
  olcPGE::olcPGE
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
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
