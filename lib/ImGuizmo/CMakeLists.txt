cmake_minimum_required(VERSION 3.13)
project(ImGuizmo)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
  ImGuizmo.h
  ImGuizmo.cpp)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(ImGuizmo ${SOURCE_FILES})

target_include_directories(ImGuizmo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(ImGuizmo PUBLIC imgui)

# if (${BUILD_SHARED_LIBS})
  # if (WIN32)
    # target_compile_definitions(ImGuizmo PRIVATE IMGUI_API="__declspec(dllexport)")
    # target_compile_definitions(ImGuizmo INTERFACE IMGUI_API="__declspec(dllimport)")
  # else ()
    # target_compile_definitions(ImGuizmo PUBLIC IMGUI_API="__attribute__((visibility("default")))")
  # endif()
# endif()
