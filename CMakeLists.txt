cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET Casqadium)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

# For developing only
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(${TARGET})
target_sources(${TARGET} PRIVATE

  src/Casqadium.cpp
  include/Casqadium.hpp

  src/CasqadiumState.cpp
  include/CasqadiumState.hpp

  src/CasqadiumStateDemo.cpp
  include/CasqadiumStateDemo.hpp

  src/demo/callbacks.cpp
  include/demo/callbacks.hpp

  src/demo/components.cpp
  include/demo/components.hpp

  src/demo/systems.cpp
  include/demo/systems.hpp

  src/demo/sequences/CameraFovInterpolated.cpp
  src/demo/sequences/CameraFovInterpolated_ui.cpp
  include/demo/sequences/CameraFovInterpolated.hpp

  src/main.cpp
  src/icon.rc
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
)

target_compile_options(${TARGET} PUBLIC -Og)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#  target_link_libraries(${TARGET} PUBLIC -mconsole)
  set_target_properties(${TARGET} PROPERTIES
    WIN32_EXECUTABLE FALSE
  )
else()
#  target_link_libraries(${TARGET} PUBLIC -mwindows)
  set_target_properties(${TARGET} PROPERTIES
    WIN32_EXECUTABLE TRUE
  )
endif()

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

include(GNUInstallDirs)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

set(IMGUI_BUILD_BACKEND_OPENGL_2 ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_DEBUG_TOOLS ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_DEMO_WINDOWS ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_OBSOLETE_FUNCTIONS ON CACHE BOOL "" FORCE)

add_subdirectory(lib)

target_link_libraries(${TARGET} PRIVATE
  casqadium::casqadium
)

if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

endif()

target_include_directories(${TARGET} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
file(COPY data DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
