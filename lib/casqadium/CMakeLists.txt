cmake_minimum_required(VERSION 3.8)

set(TARGET casqadium)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

set(CASQADIUM_SHORT cqde)

option(CQDE_DISABLE_SIMD "Disable SIMD instructions usage in Casqadium" OFF)
option(GLM_FORCE_INLINE "Force GLM functions inlining" OFF)

target_sources(${TARGET} PRIVATE

  include/${CASQADIUM_SHORT}/alias.hpp
  include/${CASQADIUM_SHORT}/fwd.hpp
  include/${CASQADIUM_SHORT}/rp3d_fwd.hpp

  include/${CASQADIUM_SHORT}/callbacks.hpp
  include/${CASQADIUM_SHORT}/systems.hpp

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

  src/common.cpp
  include/${CASQADIUM_SHORT}/common.hpp

  src/identifier.cpp
  include/${CASQADIUM_SHORT}/identifier.hpp

  src/util/logger.cpp
  include/${CASQADIUM_SHORT}/util/logger.hpp

  src/util/logger_rp3d.cpp
  include/${CASQADIUM_SHORT}/util/logger_rp3d.hpp

  src/util/editor_imgui_settings.cpp
  include/${CASQADIUM_SHORT}/util/editor_imgui_settings.hpp

  src/conversion/json_glm_vec.cpp
  src/conversion/rp3d_glm.cpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec1.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec2.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec3.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_vec4.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_quat.hpp
  include/${CASQADIUM_SHORT}/conversion/json_glm_bvec3.hpp
  include/${CASQADIUM_SHORT}/conversion/rp3d_glm.hpp

  src/conversion/enum_strings.cpp
  include/${CASQADIUM_SHORT}/conversion/enum_strings.hpp

  src/conversion/soloud_string.cpp
  include/${CASQADIUM_SHORT}/conversion/soloud_string.hpp


#  Classes & PODs
  include/${CASQADIUM_SHORT}/types/enums.hpp

  include/${CASQADIUM_SHORT}/types/FrameCurrent.hpp
  include/${CASQADIUM_SHORT}/types/System.hpp
  include/${CASQADIUM_SHORT}/types/Terrain.hpp
  include/${CASQADIUM_SHORT}/types/TickCurrent.hpp

  include/${CASQADIUM_SHORT}/types/UndoRedoQueue.hpp
  include/${CASQADIUM_SHORT}/types/UndoRedoQueue-inl.hpp

  src/types/UndoRedoQueue_types.cpp
  include/${CASQADIUM_SHORT}/types/UndoRedoQueue_types.hpp

  src/types/Package.cpp
  src/types/Package_ui.cpp
  include/${CASQADIUM_SHORT}/types/Package.hpp

  src/types/SplineCurve.cpp
  include/${CASQADIUM_SHORT}/types/SplineCurve.hpp

  src/types/TimeProgress.cpp
  src/types/TimeProgress_ui.cpp
  include/${CASQADIUM_SHORT}/types/TimeProgress.hpp

  src/types/VertexBuffer.cpp
  include/${CASQADIUM_SHORT}/types/VertexBuffer.hpp

  src/types/physics/PhysicsEventListener.cpp
  include/${CASQADIUM_SHORT}/types/physics/PhysicsEventListener.hpp

  src/types/physics/ColliderFactory.cpp
  include/${CASQADIUM_SHORT}/types/physics/ColliderFactory.hpp

  src/types/physics/colliders/Collider.cpp
  src/types/physics/colliders/Collider_ui.cpp
  include/${CASQADIUM_SHORT}/types/physics/colliders/Collider.hpp

  src/types/physics/colliders/ColliderBox.cpp
  src/types/physics/colliders/ColliderBox_ui.cpp
  include/${CASQADIUM_SHORT}/types/physics/colliders/ColliderBox.hpp

  src/types/physics/colliders/ColliderCapsule.cpp
  src/types/physics/colliders/ColliderCapsule_ui.cpp
  include/${CASQADIUM_SHORT}/types/physics/colliders/ColliderCapsule.hpp

  src/types/physics/colliders/ColliderHeightField.cpp
  src/types/physics/colliders/ColliderHeightField_ui.cpp
  include/${CASQADIUM_SHORT}/types/physics/colliders/ColliderHeightField.hpp

  src/types/physics/colliders/ColliderSphere.cpp
  src/types/physics/colliders/ColliderSphere_ui.cpp
  include/${CASQADIUM_SHORT}/types/physics/colliders/ColliderSphere.hpp


#  Sequences
  src/types/sequences/SequenceStep.cpp
  include/${CASQADIUM_SHORT}/types/sequences/SequenceStep.hpp

  src/types/sequences/SequenceFactory.cpp
  include/${CASQADIUM_SHORT}/types/sequences/SequenceFactory.hpp

  src/types/sequences/CameraFovInterpolated.cpp
  src/types/sequences/CameraFovInterpolated_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/CameraFovInterpolated.hpp

  src/types/sequences/Delay.cpp
  src/types/sequences/Delay_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/Delay.hpp

  src/types/sequences/CallbackExecute.cpp
  src/types/sequences/CallbackExecute_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/CallbackExecute.hpp

  src/types/sequences/CallbackExecuteAPeriodic.cpp
  src/types/sequences/CallbackExecuteAPeriodic_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/CallbackExecuteAPeriodic.hpp

  src/types/sequences/CallbackExecutePeriodic.cpp
  src/types/sequences/CallbackExecutePeriodic_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/CallbackExecutePeriodic.hpp

  src/types/sequences/RandomDelay.cpp
  src/types/sequences/RandomDelay_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/RandomDelay.hpp

  src/types/sequences/TextureTintInterpolated.cpp
  src/types/sequences/TextureTintInterpolated_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/TextureTintInterpolated.hpp

  src/types/sequences/TransformInterpolated.cpp
  src/types/sequences/TransformInterpolated_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/TransformInterpolated.hpp

  src/types/sequences/TransformManipulate.cpp
  src/types/sequences/TransformManipulate_ui.cpp
  include/${CASQADIUM_SHORT}/types/sequences/TransformManipulate.hpp


#  Managers
  src/types/CallbackManager.cpp
  include/${CASQADIUM_SHORT}/types/CallbackManager.hpp

  src/types/ConfigManager.cpp
  include/${CASQADIUM_SHORT}/types/ConfigManager.hpp

  src/types/EntityManager.cpp
  include/${CASQADIUM_SHORT}/types/EntityManager.hpp

  src/types/EntityReference.cpp
  include/${CASQADIUM_SHORT}/types/EntityReference.hpp

  src/types/PackageManager.cpp
  include/${CASQADIUM_SHORT}/types/PackageManager.hpp

  src/types/PhysicsManager.cpp
  include/${CASQADIUM_SHORT}/types/PhysicsManager.hpp

  src/types/PrefabManager.cpp
  include/${CASQADIUM_SHORT}/types/PrefabManager.hpp

  src/types/SnapshotManager.cpp
  include/${CASQADIUM_SHORT}/types/SnapshotManager.hpp

  src/types/SystemManager.cpp
  include/${CASQADIUM_SHORT}/types/SystemManager.hpp

  src/types/UserManager.cpp
  include/${CASQADIUM_SHORT}/types/UserManager.hpp


#  Assets
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

  src/types/assets/TerrainAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TerrainAssetManager.hpp

  src/types/assets/TextureAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TextureAssetManager.hpp


#  Input
  src/types/input/InputManager.cpp
  include/${CASQADIUM_SHORT}/types/input/InputManager.hpp

  src/types/input/ControlAxis.cpp
  src/types/input/ControlAxis_ui.cpp
  include/${CASQADIUM_SHORT}/types/input/ControlAxis.hpp

  src/types/input/InputBinding.cpp
  src/types/input/InputBinding_ui.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBinding.hpp

  src/types/input/InputBindingAbsolute.cpp
  src/types/input/InputBindingAbsolute_ui.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBindingAbsolute.hpp

  src/types/input/InputBindingRelative.cpp
  include/${CASQADIUM_SHORT}/types/input/InputBindingRelative.hpp

  include/${CASQADIUM_SHORT}/types/input/InputEvent.hpp


#  UI implementation
  src/types/ui/AssetManagerUi.cpp
  include/${CASQADIUM_SHORT}/types/ui/AssetManagerUi.hpp

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

#  UI widgets
  src/types/ui/widgets/ToggleButton.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/ToggleButton.hpp

  src/types/ui/widgets/StringFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/StringFilter.hpp

  src/types/ui/widgets/PackageFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/PackageFilter.hpp

  src/types/ui/widgets/RegistryFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/RegistryFilter.hpp


#  Components
  src/components/Camera.cpp
  src/components/Camera_ui.cpp
  include/${CASQADIUM_SHORT}/components/Camera.hpp

  src/components/CasqadiumEditorCameraSettings.cpp
  src/components/CasqadiumEditorCameraSettings_ui.cpp
  include/${CASQADIUM_SHORT}/components/CasqadiumEditorCameraSettings.hpp

  src/components/CasqadiumEditorInternal.cpp
  include/${CASQADIUM_SHORT}/components/CasqadiumEditorInternal.hpp

  src/components/CasqadiumEntryPoint.cpp
  include/${CASQADIUM_SHORT}/components/CasqadiumEntryPoint.hpp

  src/components/EntityList.cpp
  src/components/EntityList_ui.cpp
  include/${CASQADIUM_SHORT}/components/EntityList.hpp

  src/components/EntityMetaInfo.cpp
  src/components/EntityMetaInfo_ui.cpp
  include/${CASQADIUM_SHORT}/components/EntityMetaInfo.hpp

  src/components/GeometryBuffer.cpp
  src/components/GeometryBuffer_ui.cpp
  include/${CASQADIUM_SHORT}/components/GeometryBuffer.hpp

  src/components/InputController.cpp
  src/components/InputController_ui.cpp
  include/${CASQADIUM_SHORT}/components/InputController.hpp

  src/components/InteractionListener.cpp
  src/components/InteractionListener_ui.cpp
  include/${CASQADIUM_SHORT}/components/InteractionListener.hpp

  src/components/InteractionListenerColor.cpp
  src/components/InteractionListenerColor_ui.cpp
  include/${CASQADIUM_SHORT}/components/InteractionListenerColor.hpp

  src/components/InteractionProbe.cpp
  src/components/InteractionProbe_ui.cpp
  include/${CASQADIUM_SHORT}/components/InteractionProbe.hpp

  src/components/InteractionSource.cpp
  src/components/InteractionSource_ui.cpp
  include/${CASQADIUM_SHORT}/components/InteractionSource.hpp

  src/components/InteractionSourceActiveAction.cpp
  src/components/InteractionSourceActiveAction_ui.cpp
  include/${CASQADIUM_SHORT}/components/InteractionSourceActiveAction.hpp

  src/components/LightSource.cpp
  src/components/LightSource_ui.cpp
  include/${CASQADIUM_SHORT}/components/LightSource.hpp

  src/components/LightTarget.cpp
  src/components/LightTarget_ui.cpp
  include/${CASQADIUM_SHORT}/components/LightTarget.hpp

  src/components/SceneNode.cpp
  src/components/SceneNode_ui.cpp
  include/${CASQADIUM_SHORT}/components/SceneNode.hpp

  src/components/SequenceController.cpp
  src/components/SequenceController_ui.cpp
  include/${CASQADIUM_SHORT}/components/SequenceController.hpp

  src/components/SnapshotExcluded.cpp
  include/${CASQADIUM_SHORT}/components/SnapshotExcluded.hpp

  src/components/SubscriberInput.cpp
  include/${CASQADIUM_SHORT}/components/SubscriberInput.hpp

  src/components/SubscriberUpdate.cpp
  include/${CASQADIUM_SHORT}/components/SubscriberUpdate.hpp

  src/components/SystemList.cpp
  src/components/SystemList_ui.cpp
  include/${CASQADIUM_SHORT}/components/SystemList.hpp

  src/components/Tag.cpp
  src/components/Tag_ui.cpp
  include/${CASQADIUM_SHORT}/components/Tag.hpp

  src/components/TextureBuffer.cpp
  src/components/TextureBuffer_ui.cpp
  include/${CASQADIUM_SHORT}/components/TextureBuffer.hpp

  src/components/TextureTint.cpp
  src/components/TextureTint_ui.cpp
  include/${CASQADIUM_SHORT}/components/TextureTint.hpp

  src/components/Transform.cpp
  src/components/Transform_ui.cpp
  include/${CASQADIUM_SHORT}/components/Transform.hpp

  src/components/WantsMouseCentered.cpp
  include/${CASQADIUM_SHORT}/components/WantsMouseCentered.hpp

  src/components/WantsMouseHidden.cpp
  include/${CASQADIUM_SHORT}/components/WantsMouseHidden.hpp


  src/components/assets/AudioAssetList.cpp
  src/components/assets/AudioAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/AudioAssetList.hpp

  src/components/assets/FontAssetList.cpp
  src/components/assets/FontAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/FontAssetList.hpp

  src/components/assets/GeometryAssetList.cpp
  src/components/assets/GeometryAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/GeometryAssetList.hpp

  src/components/assets/TerrainAssetList.cpp
  src/components/assets/TerrainAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/TerrainAssetList.hpp

  src/components/assets/TextStringAssetList.cpp
  src/components/assets/TextStringAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/TextStringAssetList.hpp

  src/components/assets/TextureAssetList.cpp
  src/components/assets/TextureAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/TextureAssetList.hpp


  src/components/physics/CollisionBody.cpp
  src/components/physics/CollisionBody_ui.cpp
  include/${CASQADIUM_SHORT}/components/physics/CollisionBody.hpp

  src/components/physics/RigidBody.cpp
  src/components/physics/RigidBody_ui.cpp
  include/${CASQADIUM_SHORT}/components/physics/RigidBody.hpp

  src/components/physics/ForceEmitter.cpp
  src/components/physics/ForceEmitter_ui.cpp
  include/${CASQADIUM_SHORT}/components/physics/ForceEmitter.hpp


#  Callbacks
  src/callbacks/assets_routine.cpp
  include/${CASQADIUM_SHORT}/callbacks/assets_routine.hpp

  src/callbacks/editor_common.cpp
  include/${CASQADIUM_SHORT}/callbacks/editor_common.hpp

  src/callbacks/editor_camera_input.cpp
  include/${CASQADIUM_SHORT}/callbacks/editor_camera_input.hpp

  src/callbacks/editor_gizmo_input.cpp
  include/${CASQADIUM_SHORT}/callbacks/editor_gizmo_input.hpp

  src/callbacks/editor_input.cpp
  include/${CASQADIUM_SHORT}/callbacks/editor_input.hpp

  src/callbacks/common_routine.cpp
  include/${CASQADIUM_SHORT}/callbacks/common_routine.hpp

  src/callbacks/input.cpp
  include/${CASQADIUM_SHORT}/callbacks/input.hpp

  src/callbacks/physics.cpp
  include/${CASQADIUM_SHORT}/callbacks/physics.hpp


#  Systems
  src/systems/input.cpp
  include/${CASQADIUM_SHORT}/systems/input.hpp

  src/systems/interaction.cpp
  include/${CASQADIUM_SHORT}/systems/interaction.hpp

  src/systems/logic.cpp
  include/${CASQADIUM_SHORT}/systems/logic.hpp

  src/systems/physics.cpp
  include/${CASQADIUM_SHORT}/systems/physics.hpp

  src/systems/render.cpp
  include/${CASQADIUM_SHORT}/systems/render.hpp
)

set_target_properties(${TARGET} PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
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

set(TBB_TEST OFF CACHE BOOL "" FORCE) # TBB
set(TBB_STRICT OFF CACHE BOOL "" FORCE)
set(TBBMALLOC_BUILD OFF CACHE BOOL "" FORCE)
set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON CACHE BOOL "" FORCE)

set(RP3D_DOUBLE_PRECISION_ENABLED OFF CACHE BOOL "" FORCE) # ReactPhysics3D


include(FetchContent)

FetchContent_Declare(ctpl
  GIT_REPOSITORY git@github.com:Casqade/CTPL
  GIT_TAG        master
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(ctpl)


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


if (NOT ${CQDE_DISABLE_SIMD})
  FetchContent_Declare(tbb
    GIT_REPOSITORY https://github.com/oneapi-src/oneTBB
    GIT_TAG        v2021.7.0
    GIT_SHALLOW    TRUE
  )
  FetchContent_MakeAvailable(tbb)
endif()


FetchContent_Declare(ReactPhysics3D
  GIT_REPOSITORY https://github.com/Casqade/reactphysics3d
  GIT_TAG        master
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(ReactPhysics3D)


FetchContent_Declare(soloud
  GIT_REPOSITORY https://github.com/Casqade/soloud
  GIT_TAG        master
  GIT_SHALLOW    TRUE
  SOURCE_SUBDIR  contrib
)
FetchContent_MakeAvailable(soloud)


if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(EnTT INTERFACE
    ENTT_DISABLE_ASSERT
  )
endif()

set(GLM_LIB_TYPE glm_static)
set(JSONCPP_LIB_TYPE jsoncpp_static)

if(${BUILD_SHARED_LIBS})
  set(GLM_LIB_TYPE glm_shared)
  set(JSONCPP_LIB_TYPE jsoncpp_lib)
endif()

target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
  GLM_FORCE_DEPTH_ZERO_TO_ONE
)

if(${GLM_FORCE_INLINE})
  target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
    GLM_FORCE_INLINE
  )
endif()

if(NOT ${CQDE_DISABLE_SIMD})
  target_compile_options(${GLM_LIB_TYPE} PUBLIC
    -msse4.1
  )
  target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
    GLM_FORCE_INTRINSICS
    GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
  )

  target_compile_options(tbb PUBLIC
    -msse4.1
  )

  target_compile_definitions(tbb PUBLIC
    TBB_USE_PROFILING_TOOLS=0
  )

  target_link_libraries(${TARGET} PUBLIC
    TBB::tbb
  )
endif()


target_link_libraries(${TARGET} PUBLIC
  ctpl::ctpl
  TimeUtils::TimeUtils
  olcPGE::olcPGE
  EnTT::EnTT
  glm::${GLM_LIB_TYPE}
  ${JSONCPP_LIB_TYPE}
  spdlog::spdlog
  ReactPhysics3D::reactphysics3d
  soloud
)


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
