cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(TARGET olcPGE)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
  include/${TARGET}/olcPixelGameEngine.hpp
  src/olcPixelGameEngine.cpp

  include/${TARGET}/olcPGEX_CustomFont.hpp
  src/olcPGEX_CustomFont.cpp

  include/${TARGET}/olcPGEX_TTF.hpp
  src/olcPGEX_TTF.cpp

  imgui/imgui.h
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/backends/imgui_impl_opengl2.h
  imgui/backends/imgui_impl_opengl2.cpp

  ImGuizmo/ImGuizmo.h
  ImGuizmo/ImGuizmo.cpp

  include/${TARGET}/olcPGEX_ImGui.hpp
  src/olcPGEX_ImGui.cpp

  include/${TARGET}/olcEvent.hpp

  include/${TARGET}/olcEventHandler.hpp
  src/olcEventHandler.cpp

  include/${TARGET}/olcMouseInputId.hpp
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
)

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

find_package(Freetype REQUIRED)
target_link_libraries(${TARGET} PUBLIC Freetype::Freetype)

find_package(glm REQUIRED)
target_link_libraries(${TARGET} PUBLIC glm)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

  target_link_libraries(${TARGET} PUBLIC user32 gdi32 opengl32 gdiplus Shlwapi dwmapi)

else()
  target_link_libraries(${TARGET} PRIVATE X11 GL png)
endif()

include(GNUInstallDirs)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/imgui>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ImGuizmo>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
