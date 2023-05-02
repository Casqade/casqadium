cmake_minimum_required(VERSION 3.13)

set(TARGET ImGuizmo)
set(${TARGET}_VERSION 1.83)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(${TARGET})

target_sources(${TARGET} PRIVATE
  ImGuizmo.cpp
  ImGuizmo.h
)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(${TARGET} PUBLIC imgui)

if (${BUILD_SHARED_LIBS})
 if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   target_compile_definitions(${TARGET} PRIVATE "IMGUI_API=__declspec(dllexport)")
 endif()
endif()
