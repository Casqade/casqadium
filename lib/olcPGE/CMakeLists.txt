cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(TARGET olcPGE)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

option(${TARGET}_USE_OPENGL33 "${TARGET}: Use OpenGL 3.3" OFF)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

target_sources(${TARGET} PRIVATE
  include/${TARGET}/olcPixelGameEngine.hpp
  src/olcPixelGameEngine.cpp

  include/${TARGET}/olcPGEX_CustomFont.hpp
  src/olcPGEX_CustomFont.cpp

  include/${TARGET}/olcPGEX_TTF.hpp
  src/olcPGEX_TTF.cpp

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

if(${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
else()
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  target_link_libraries(${TARGET} PUBLIC -static)
endif()


include(FetchContent)

FetchContent_Declare(Freetype
  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
  GIT_TAG        VER-2-12-1
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Freetype)

target_compile_definitions(freetype PRIVATE FT_CONFIG_OPTION_ERROR_STRINGS)
target_link_libraries(${TARGET} PUBLIC freetype)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

target_link_libraries(${TARGET} PUBLIC imgui imgui_backend_opengl2)


if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

  target_link_libraries(${TARGET} PUBLIC user32 gdi32 opengl32 gdiplus Shlwapi dwmapi)

  if(${TARGET}_USE_OPENGL33)
    find_package(glew REQUIRED)

    target_compile_definitions(${TARGET} PUBLIC OLC_GFX_OPENGL33)

    if(${BUILD_SHARED_LIBS})
     target_link_libraries(${TARGET} PUBLIC GLEW::glew)
    else()
     target_link_libraries(${TARGET} PUBLIC GLEW::glew_s)
    endif()
  endif()

else()
  target_link_libraries(${TARGET} PRIVATE X11 GL png)
endif()

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/${TARGET}/${TARGET}_export.hpp)

include(GNUInstallDirs)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ImGuizmo>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_BINARY_DIR}/exports/${TARGET}/${TARGET}_export.hpp
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}"
)

