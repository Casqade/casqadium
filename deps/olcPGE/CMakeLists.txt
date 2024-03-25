cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(TARGET olcPGE)
project(${TARGET} VERSION 2.16 LANGUAGES CXX)

add_library(${TARGET})

option(OLC_USE_STB_IMAGE "Use stb_image.h for sprite loading" OFF)

target_sources(${TARGET} PRIVATE

  include/${TARGET}/v2d_generic.hpp
  src/v2d_generic.cpp

  include/${TARGET}/Pixel.hpp
  src/Pixel.cpp

  include/${TARGET}/Sprite.hpp
  src/Sprite.cpp

  include/${TARGET}/Decal.hpp
  src/Decal.cpp

  include/${TARGET}/Renderable.hpp
  src/Renderable.cpp

  include/${TARGET}/ImageLoader.hpp
  src/ImageLoader.cpp

  include/${TARGET}/Font.hpp
  src/Font.cpp

  src/fonts/ProggyClean.cpp
  src/fonts/RobotoMedium.cpp
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

if(${OLC_USE_STB_IMAGE})
  target_compile_definitions(${TARGET} PRIVATE OLC_IMAGE_STB)

elseif(WIN32)
  target_compile_definitions(${TARGET} PRIVATE OLC_IMAGE_GDI)

else()
  target_compile_definitions(${TARGET} PRIVATE OLC_IMAGE_LIBPNG)
endif()


set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)
set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
set(FT_ENABLE_ERROR_STRINGS ON CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(Freetype
#  GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
  GIT_REPOSITORY https://github.com/freetype/freetype
  GIT_TAG        VER-2-13-2
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(Freetype)

if(${BUILD_SHARED_LIBS} AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_definitions(freetype PRIVATE DLL_EXPORT)
  target_compile_definitions(freetype INTERFACE DLL_IMPORT)
endif()

target_link_libraries(${TARGET} PUBLIC
  freetype
  glad
  glfw
)


if(WIN32)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  else()
    if(NOT ${BUILD_SHARED_LIBS})
      target_link_libraries(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
    endif()
  endif()

  if(NOT ${OLC_USE_STB_IMAGE})
    target_link_libraries(${TARGET} PUBLIC gdiplus)
  endif()

elseif(NOT ${OLC_USE_STB_IMAGE})
  target_link_libraries(${TARGET} PRIVATE GL png)
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

