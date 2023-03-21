cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

set(TARGET CasqadiumApp)
project(${TARGET} VERSION 1.0 LANGUAGES CXX)

# For developing only
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_executable(${TARGET})
target_sources(${TARGET} PRIVATE

  app/main.cpp
  app/icon.rc
)

if(CMAKE_BUILD_TYPE MATCHES "Release")
  target_compile_options(${TARGET} PUBLIC -O2)
  #  target_link_libraries(${TARGET} PUBLIC -mwindows)
  set_target_properties(${TARGET} PROPERTIES
    WIN32_EXECUTABLE TRUE
  )
else()
  target_compile_options(${TARGET} PUBLIC -Og)
#  target_link_libraries(${TARGET} PUBLIC -mconsole)
  set_target_properties(${TARGET} PROPERTIES
    WIN32_EXECUTABLE FALSE
  )
endif()

if(NOT ${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC -static)
endif()

include(GNUInstallDirs)

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
