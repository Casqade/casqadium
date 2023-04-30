cmake_minimum_required(VERSION 3.13)

set(TARGET imgui)
set(${TARGET}_VERSION 1.89.1)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

option(IMGUI_BUILD_BACKEND_GLFW "Build backend GLFW" OFF)
option(IMGUI_BUILD_BACKEND_OPENGL_2 "Build backend OPENGL 2" OFF)
option(IMGUI_BUILD_BACKEND_OPENGL_3 "Build backend OPENGL 3" OFF)

option(IMGUI_ENABLE_OPENGL_DEBUG "Report OpenGL errors" OFF)
option(IMGUI_DISABLE_DEBUG_TOOLS "Disable metrics/debugger and other debug tools" OFF)
option(IMGUI_DISABLE_DEMO_WINDOWS "Disable ShowDemoWindow()/ShowStyleEditor()" OFF)
option(IMGUI_DISABLE_OBSOLETE_FUNCTIONS "Disable obsolete functions" OFF)
option(IMGUI_DISABLE_OBSOLETE_KEYIO "Disable obsolete key values" OFF)

add_library(imgui)

target_sources(imgui PRIVATE
  imgui.h
  imconfig.h
  imgui_internal.h
  imstb_rectpack.h
  imstb_textedit.h
  imstb_truetype.h

  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_tables.cpp
  imgui_widgets.cpp

  misc/cpp/imgui_stdlib.h
  misc/cpp/imgui_stdlib.cpp

  misc/curve/imgui_bezier.hpp
  misc/curve/imgui_bezier.cpp
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 11
)

if (${IMGUI_DISABLE_DEBUG_TOOLS})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_DEBUG_TOOLS)
endif()

if (${IMGUI_DISABLE_DEMO_WINDOWS})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_DEMO_WINDOWS)
endif()

if (${IMGUI_DISABLE_OBSOLETE_FUNCTIONS})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
endif()

if (${IMGUI_DISABLE_OBSOLETE_KEYIO})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_KEYIO)
endif()

target_include_directories(imgui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/backends>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/misc/cpp>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/misc/curve>
)

if (${BUILD_SHARED_LIBS})
 if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_definitions(imgui PRIVATE "IMGUI_API=__declspec(dllexport)")
   target_compile_definitions(imgui INTERFACE "IMGUI_API=__declspec(dllimport)")
 endif()
endif()

if(${IMGUI_BUILD_BACKEND_GLFW})
    add_library(imgui_backend_glfw ./backends/imgui_impl_glfw.cpp)
    target_link_libraries(imgui_backend_glfw PUBLIC imgui glfw)
endif()

if(${IMGUI_BUILD_BACKEND_OPENGL_2})
    add_library(imgui_backend_opengl2 ./backends/imgui_impl_opengl2.cpp)
    target_link_libraries(imgui_backend_opengl2 PUBLIC imgui)
endif()

if(${IMGUI_BUILD_BACKEND_OPENGL_3})
    add_library(imgui_backend_opengl3 ./backends/imgui_impl_opengl3.cpp)

    if (${IMGUI_ENABLE_OPENGL_DEBUG})
      target_compile_definitions(imgui_backend_opengl3 PRIVATE IMGUI_IMPL_OPENGL_DEBUG)
    endif()

    target_link_libraries(imgui_backend_opengl3 PUBLIC imgui)
endif()
