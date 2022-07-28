cmake_minimum_required(VERSION 3.13)
project(imgui)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
  imgui.cpp
  imgui_draw.cpp
  imgui_demo.cpp
  imgui_tables.cpp
  imgui_widgets.cpp)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

option(IMGUI_DISABLE "Disable " OFF)
option(IMGUI_DISABLE_OBSOLETE_FUNCTIONS "Disable obsolete functions" OFF)
option(IMGUI_DISABLE_DEMO_WINDOWS "Disable ShowDemoWindow()/ShowStyleEditor()" OFF)
option(IMGUI_DISABLE_DEBUG_TOOLS "Disable metrics/debugger and other debug tools" OFF)

option(IMGUI_BUILD_BACKEND_DX_11 "Build backend directX 11" OFF)
option(IMGUI_BUILD_BACKEND_GLFW "Build backend GLFW" OFF)
option(IMGUI_BUILD_BACKEND_OPENGL_2 "Build backend OPENGL 2" OFF)
option(IMGUI_BUILD_BACKEND_OPENGL_3 "Build backend OPENGL 3" OFF)

add_library(imgui ${SOURCE_FILES})

if (${IMGUI_DISABLE_OBSOLETE_FUNCTIONS})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
endif()

if (${IMGUI_DISABLE_DEMO_WINDOWS})
  target_compile_definitions(imgui PUBLIC IMGUI_DISABLE_DEMO_WINDOWS)
endif()

target_include_directories(imgui PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/backends>
)

# if (${BUILD_SHARED_LIBS})
  # if (WIN32)
    # target_compile_definitions(imgui PRIVATE IMGUI_API="__declspec(dllexport)")
    # target_compile_definitions(imgui INTERFACE IMGUI_API="__declspec(dllimport)")
  # else ()
    # target_compile_definitions(imgui PUBLIC IMGUI_API="__attribute__((visibility("default")))")
  # endif()
# endif()

if(${IMGUI_BUILD_BACKEND_DX_11})
    set(BACKEND_FILE ./backends/imgui_impl_dx11.cpp)
    add_library(imgui_backend_dx11 ${BACKEND_FILE})
    target_link_libraries(imgui_backend_dx11 PUBLIC imgui)
endif()
if(${IMGUI_BUILD_BACKEND_GLFW})
    set(BACKEND_FILE ./backends/imgui_impl_glfw.cpp)
    add_library(imgui_backend_glfw ${BACKEND_FILE})
    target_link_libraries(imgui_backend_glfw PUBLIC imgui)
endif()
if(${IMGUI_BUILD_BACKEND_OPENGL_2})
    find_package(OpenGL REQUIRED)

    set(BACKEND_FILE ./backends/imgui_impl_opengl2.cpp)
    add_library(imgui_backend_opengl2 ${BACKEND_FILE})
    target_link_libraries(imgui_backend_opengl2 PUBLIC OpenGL::GL imgui)
endif()
if(${IMGUI_BUILD_BACKEND_OPENGL_3})
    set(BACKEND_FILE ./backends/imgui_impl_opengl3.cpp)
    add_library(imgui_backend_opengl3 ${BACKEND_FILE})
    target_link_libraries(imgui_backend_opengl3 PUBLIC imgui)
endif()
