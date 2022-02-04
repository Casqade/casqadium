cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET AnotherDayAtHospital)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# For developing only
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(${TARGET})
target_sources(${TARGET} PRIVATE

  include/Variables.hpp

  include/Assets.hpp
  include/Logger.hpp
  include/Widgets.hpp
  include/AnotherDayAtHospital.hpp

  src/Assets.cpp
  src/Widgets.cpp
  src/Logger.cpp

  src/AnotherDayAtHospital.cpp

  src/main.cpp
  src/icon.rc
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
)

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

add_subdirectory(GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

target_link_libraries(${TARGET} PRIVATE TimeUtils::TimeUtils)
target_link_libraries(${TARGET} PRIVATE Graphics3D::Graphics3D)
target_link_libraries(${TARGET} PRIVATE olcPGE::olcPGE)

include(GNUInstallDirs)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

find_package(SFML COMPONENTS audio QUIET)

if(NOT SFML_FOUND)
  include(FetchContent)

  FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML
    GIT_TAG        2.5.1
    GIT_SHALLOW    TRUE
  )

  set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_AUDIO ON CACHE BOOL "" FORCE)
  set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
  set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
  set(SFML_USE_SYSTEM_DEPS OFF CACHE BOOL "" FORCE)

  if(${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SFML_USE_STATIC_STD_LIBS ON CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(SFML)

endif()

target_link_libraries(${TARGET} PRIVATE sfml-audio)
target_compile_options(${TARGET} PRIVATE -Og)
if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

endif()

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
file(COPY assets DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
