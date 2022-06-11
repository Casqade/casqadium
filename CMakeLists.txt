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

  include/AnotherDayAtHospital.hpp

  src/AnotherDayAtHospital.cpp

  src/main.cpp
  src/icon.rc
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 14
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

add_subdirectory(GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/GameStates)
target_include_directories(${TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

include(GNUInstallDirs)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} PRIVATE Threads::Threads)

add_subdirectory(lib)

target_link_libraries(${TARGET} PRIVATE
  TimeUtils::TimeUtils
  olcPGE::olcPGE
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

install(TARGETS ${TARGET} DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
file(COPY data DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})
