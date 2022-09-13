cmake_minimum_required(VERSION 3.8)

set(TARGET casqadium)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

set(CASQADIUM_SHORT cqde)

target_sources(${TARGET} PRIVATE
  include/${CASQADIUM_SHORT}/alias.hpp
  include/${CASQADIUM_SHORT}/fwd.hpp

  src/ecs_helpers.cpp
  include/${CASQADIUM_SHORT}/ecs_helpers.hpp

  src/file_helpers.cpp
  include/${CASQADIUM_SHORT}/file_helpers.hpp

  src/json_helpers.cpp
  include/${CASQADIUM_SHORT}/json_helpers.hpp

  src/math_helpers.cpp
  include/${CASQADIUM_SHORT}/math_helpers.hpp

  src/render_helpers.cpp
  include/${CASQADIUM_SHORT}/render_helpers.hpp

  src/callbacks.cpp
  include/${CASQADIUM_SHORT}/callbacks.hpp

  src/common.cpp
  include/${CASQADIUM_SHORT}/common.hpp

  src/identifier.cpp
  include/${CASQADIUM_SHORT}/identifier.hpp

  src/systems.cpp
  include/${CASQADIUM_SHORT}/systems.hpp

  src/conversion/json_glm_vec.cpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec1.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec2.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec3.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec4.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_quat.hpp

  src/types/CallbackManager.cpp
  include/${CASQADIUM_SHORT}/types/CallbackManager.hpp

  src/types/EntityManager.cpp
  include/${CASQADIUM_SHORT}/types/EntityManager.hpp

  src/types/EntityReference.cpp
  include/${CASQADIUM_SHORT}/types/EntityReference.hpp

  src/types/SystemManager.cpp
  include/${CASQADIUM_SHORT}/types/SystemManager.hpp

  src/types/input/InputManager.cpp
  include/${CASQADIUM_SHORT}/types/input/InputManager.hpp

  src/types/input/ControlAxis.cpp
  src/types/input/ControlAxis_ui_impl.cpp
  include/${CASQADIUM_SHORT}/types/input/ControlAxis.hpp

  src/types/input/InputBinding.cpp
  src/types/input/InputBinding_ui_impl.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBinding.hpp

  src/types/input/InputBindingAbsolute.cpp
  src/types/input/InputBindingAbsolute_ui_impl.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBindingAbsolute.hpp

  src/types/input/InputBindingRelative.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBindingRelative.hpp

  include/${CASQADIUM_SHORT}/types/input/InputEvent.hpp

  src/types/ui/AssetManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/AssetManagerUi.hpp

  src/types/ui/CallbackManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/CallbackManagerUi.hpp

  src/types/ui/EntityManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/EntityManagerUi.hpp

  src/types/ui/InputManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/InputManagerUi.hpp

  src/types/ui/PackageManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/PackageManagerUi.hpp

  src/types/ui/PrefabManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/PrefabManagerUi.hpp

  src/types/ui/ViewportManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/ViewportManagerUi.hpp

  src/types/ui/SystemManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/SystemManagerUi.hpp

  src/types/ui/widgets/ToggleButton.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/ToggleButton.hpp

  src/types/ui/widgets/StringFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/StringFilter.hpp

  src/types/ui/widgets/PackageFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/PackageFilter.hpp

  src/types/ui/widgets/RegistryFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/RegistryFilter.hpp

  include/${CASQADIUM_SHORT}/types/FrameCurrent.hpp
  include/${CASQADIUM_SHORT}/types/SequenceStep.hpp
  include/${CASQADIUM_SHORT}/types/System.hpp
  include/${CASQADIUM_SHORT}/types/TickCurrent.hpp

  include/${CASQADIUM_SHORT}/types/UndoRedoQueue.hpp
  include/${CASQADIUM_SHORT}/types/UndoRedoQueue-inl.hpp

  src/types/UndoRedoQueue_types.cpp
  include/${CASQADIUM_SHORT}/types/UndoRedoQueue_types.hpp

  src/types/VertexBuffer.cpp
  include/${CASQADIUM_SHORT}/types/VertexBuffer.hpp

  src/types/Package.cpp
  src/types/Package_ui_impl.cpp
  include/${CASQADIUM_SHORT}/types/Package.hpp

  src/types/PackageManager.cpp
  include/${CASQADIUM_SHORT}/types/PackageManager.hpp

  src/types/PrefabManager.cpp
  include/${CASQADIUM_SHORT}/types/PrefabManager.hpp

  include/${CASQADIUM_SHORT}/types/assets/AssetStatus.hpp

  include/${CASQADIUM_SHORT}/types/assets/AssetManager.hpp
  include/${CASQADIUM_SHORT}/types/assets/AssetManager-inl.hpp

  src/types/assets/AudioAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/AudioAssetManager.hpp

  src/types/assets/FontAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/FontAssetManager.hpp

  src/types/assets/GeometryAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/GeometryAssetManager.hpp

  src/types/assets/TextStringAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TextStringAssetManager.hpp

  src/types/assets/TextureAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TextureAssetManager.hpp


  src/components/Camera.cpp
  src/components/Camera_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/Camera.hpp

  src/components/CasqadiumEditorInternal.cpp
  include/${CASQADIUM_SHORT}/components/CasqadiumEditorInternal.hpp

  src/components/EntityMetaInfo.cpp
  src/components/EntityMetaInfo_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/EntityMetaInfo.hpp

  src/components/GeometryBuffer.cpp
  src/components/GeometryBuffer_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/GeometryBuffer.hpp

  src/components/InputController.cpp
  src/components/InputController_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/InputController.hpp

  src/components/CasqadiumEntryPoint.cpp
  src/components/CasqadiumEntryPoint_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/CasqadiumEntryPoint.hpp

  src/components/SceneNode.cpp
  src/components/SceneNode_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/SceneNode.hpp

  src/components/SequenceController.cpp
  src/components/SequenceController_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/SequenceController.hpp

  src/components/SnapshotExcluded.cpp
  include/${CASQADIUM_SHORT}/components/SnapshotExcluded.hpp

  src/components/SubscriberInput.cpp
  include/${CASQADIUM_SHORT}/components/SubscriberInput.hpp

  src/components/SubscriberUpdate.cpp
  include/${CASQADIUM_SHORT}/components/SubscriberUpdate.hpp

  src/components/Tag.cpp
  src/components/Tag_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/Tag.hpp

  src/components/TextureBuffer.cpp
  src/components/TextureBuffer_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/TextureBuffer.hpp

  src/components/Transform.cpp
  src/components/Transform_ui_impl.cpp
  include/${CASQADIUM_SHORT}/components/Transform.hpp


  src/util/logger.cpp
  include/${CASQADIUM_SHORT}/util/logger.hpp

  src/types/ConfigManager.cpp
  include/${CASQADIUM_SHORT}/types/ConfigManager.hpp

  thirdparty/ctpl/ctpl_stl.h
)

if(${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
  set(SPDLOG_BUILD_SHARED ON CACHE BOOL "" FORCE)
  set(BUILD_STATIC_LIBS OFF CACHE BOOL "" FORCE) # JsonCPP
  set(SOLOUD_DYNAMIC ON CACHE BOOL "" FORCE)
  set(SOLOUD_STATIC OFF CACHE BOOL "" FORCE)
  set(SOLOUD_LIBMODPLUG_SUPPORT ON CACHE BOOL "" FORCE)
  set(SOLOUD_LIBMODPLUG_SUPPORT_STATIC OFF CACHE BOOL "" FORCE)
else()
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
  set(ENTT_USE_LIBCPP ON CACHE BOOL "" FORCE)
  set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE) # JsonCpp
  set(JSONCPP_STATIC_WINDOWS_RUNTIME ON CACHE BOOL "" FORCE)
  set(SOLOUD_DYNAMIC OFF CACHE BOOL "" FORCE)
  set(SOLOUD_STATIC ON CACHE BOOL "" FORCE)
  set(SOLOUD_LIBMODPLUG_SUPPORT OFF CACHE BOOL "" FORCE)
  set(SOLOUD_LIBMODPLUG_SUPPORT_STATIC ON CACHE BOOL "" FORCE)
endif()

set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE) # JsonCPP
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF CACHE BOOL "" FORCE)

set(SPDLOG_NO_ATOMIC_LEVELS ON CACHE BOOL "" FORCE)
set(SPDLOG_DISABLE_DEFAULT_LOGGER ON CACHE BOOL "" FORCE)


include(FetchContent)

FetchContent_Declare(TimeUtils
  GIT_REPOSITORY git@gitlab.com:Casqade/timeutils.git
  GIT_TAG        main
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(TimeUtils)


FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt
  GIT_TAG        v3.10.3
  GIT_SHALLOW    TRUE
)

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
  target_compile_definitions(${TARGET} PUBLIC ENTT_DISABLE_ASSERT)
endif()

FetchContent_MakeAvailable(EnTT)


FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG        0.9.9.8
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(JsonCPP
  GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
  GIT_TAG        1.9.5
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JsonCPP)


FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        v1.10.0
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(spdlog)


FetchContent_Declare(soloud
  GIT_REPOSITORY https://github.com/Casqade/soloud
  GIT_TAG        master
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  contrib
)
FetchContent_MakeAvailable(soloud)


if(${BUILD_SHARED_LIBS})
  target_compile_definitions(glm_shared PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
#  target_compile_definitions(glm_shared PRIVATE GLM_FORCE_MESSAGES)
else()
  target_compile_definitions(glm_static PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE)
#  target_compile_definitions(glm_static PRIVATE GLM_FORCE_MESSAGES)
endif()


target_link_libraries(${TARGET} PUBLIC
  TimeUtils::TimeUtils
  EnTT::EnTT
  olcPGE::olcPGE
  spdlog::spdlog
  soloud
)

if(${BUILD_SHARED_LIBS})
  target_link_libraries(${TARGET} PUBLIC
    jsoncpp_lib
    glm::glm_shared
  )
else()
  target_link_libraries(${TARGET} PUBLIC
    jsoncpp_static
    glm::glm_static
  )
endif()

include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/${CASQADIUM_SHORT}/${CASQADIUM_SHORT}_export.hpp)

include(GNUInstallDirs)

target_include_directories(${TARGET} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/${CMAKE_INSTALL_INCLUDEDIR}>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/exports>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS ${TARGET}
  EXPORT ${TARGET} DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${CMAKE_BINARY_DIR}/exports/${CASQADIUM_SHORT}/${CASQADIUM_SHORT}_export.hpp
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${CASQADIUM_SHORT}"
)
