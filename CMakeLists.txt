cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) 
endif()

set(TARGET casqadium)
set(${TARGET}_VERSION 1.0.0)
project(${TARGET} VERSION ${${TARGET}_VERSION} LANGUAGES CXX)

add_library(${TARGET})
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

set(CASQADIUM_SHORT cqde)

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

option(CQDE_BUILD_EXAMPLE_APP "Build using casqadium example app" OFF)
option(CQDE_DISABLE_SIMD "Disable SIMD instructions usage in Casqadium and all its dependencies" OFF)
option(CQDE_DISABLE_TBB "Disable Intel TBB usage in Casqadium" ON)
option(GLM_DISABLE_SIMD "Disable SIMD instructions usage in GLM" ON)
option(GLM_FORCE_INLINE "Force GLM functions inlining" OFF)
option(SOLOUD_DISABLE_SIMD "Disable SIMD instructions usage in SoLoud" OFF)

if(${CQDE_DISABLE_SIMD})
  set(CQDE_DISABLE_TBB ON)
  set(GLM_DISABLE_SIMD ON)
  set(SOLOUD_DISABLE_SIMD ON)
endif()

target_sources(${TARGET} PRIVATE

  include/${CASQADIUM_SHORT}/alias.hpp
  include/${CASQADIUM_SHORT}/fwd.hpp
  include/${CASQADIUM_SHORT}/logger.hpp
  include/${CASQADIUM_SHORT}/rp3d_fwd.hpp

  include/${CASQADIUM_SHORT}/callbacks.hpp
  include/${CASQADIUM_SHORT}/systems.hpp


  src/audio_helpers.cpp
  include/${CASQADIUM_SHORT}/audio_helpers.hpp

  src/ecs_helpers.cpp
  include/${CASQADIUM_SHORT}/ecs_helpers.hpp

  src/file_helpers.cpp
  include/${CASQADIUM_SHORT}/file_helpers.hpp

  src/json_helpers.cpp
  include/${CASQADIUM_SHORT}/json_helpers.hpp

  src/math_helpers.cpp
  include/${CASQADIUM_SHORT}/math_helpers.hpp

  src/string_helpers.cpp
  include/${CASQADIUM_SHORT}/string_helpers.hpp

  src/render_helpers.cpp
  include/${CASQADIUM_SHORT}/render_helpers.hpp

  include/${CASQADIUM_SHORT}/assert.hpp

  src/identifier.cpp
  include/${CASQADIUM_SHORT}/identifier.hpp

  src/static_strings.cpp
  include/${CASQADIUM_SHORT}/static_strings.hpp

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

  include/${CASQADIUM_SHORT}/types/graphics/Mesh.hpp

  src/types/CasqadiumEngine.cpp
  src/types/CasqadiumEngine_audio.cpp
  src/types/CasqadiumEngine_callbacks.cpp
  src/types/CasqadiumEngine_components.cpp
  src/types/CasqadiumEngine_events.cpp
  src/types/CasqadiumEngine_logger.cpp
  src/types/CasqadiumEngine_physics.cpp
  src/types/CasqadiumEngine_sequences.cpp
  src/types/CasqadiumEngine_shaders.cpp
  src/types/CasqadiumEngine_systems.cpp
  include/${CASQADIUM_SHORT}/types/CasqadiumEngine.hpp

  src/types/UndoRedoQueue_types.cpp
  include/${CASQADIUM_SHORT}/types/UndoRedoQueue_types.hpp

  src/types/Config.cpp
  include/${CASQADIUM_SHORT}/types/Config.hpp

  src/types/LaunchParams.cpp
  include/${CASQADIUM_SHORT}/types/LaunchParams.hpp

  src/types/MouseCursor.cpp
  include/${CASQADIUM_SHORT}/types/MouseCursor.hpp

  src/types/Package.cpp
  src/types/Package_ui.cpp
  include/${CASQADIUM_SHORT}/types/Package.hpp

  src/types/SplineCurve.cpp
  include/${CASQADIUM_SHORT}/types/SplineCurve.hpp

  src/types/TimeProgress.cpp
  src/types/TimeProgress_ui.cpp
  include/${CASQADIUM_SHORT}/types/TimeProgress.hpp

  src/types/graphics/GlVertexArray.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlVertexArray.hpp

  src/types/graphics/GlBuffer.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlBuffer.hpp

  src/types/graphics/GlFramebuffer.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlFramebuffer.hpp

  src/types/graphics/GlProgram.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlProgram.hpp

  src/types/graphics/GlShader.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlShader.hpp

  src/types/graphics/GlTexture.cpp
  include/${CASQADIUM_SHORT}/types/graphics/GlTexture.hpp

  src/types/graphics/FrameReadback.cpp
  include/${CASQADIUM_SHORT}/types/graphics/FrameReadback.hpp

  src/types/graphics/MeshStorage.cpp
  include/${CASQADIUM_SHORT}/types/graphics/MeshStorage.hpp

  src/types/graphics/PrimaryRenderTarget.cpp
  include/${CASQADIUM_SHORT}/types/graphics/PrimaryRenderTarget.hpp

  src/types/graphics/RenderTarget.cpp
  include/${CASQADIUM_SHORT}/types/graphics/RenderTarget.hpp

  src/types/graphics/ShaderManager.cpp
  include/${CASQADIUM_SHORT}/types/graphics/ShaderManager.hpp

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
  src/types/assets/AssetStatus.cpp
  include/${CASQADIUM_SHORT}/types/assets/AssetStatus.hpp

  include/${CASQADIUM_SHORT}/types/assets/AssetManager.hpp
  include/${CASQADIUM_SHORT}/types/assets/AssetManager-inl.hpp

  src/types/assets/AudioAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/AudioAssetManager.hpp

  src/types/assets/FontAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/FontAssetManager.hpp

  src/types/assets/MeshAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/MeshAssetManager.hpp

  src/types/assets/MouseCursorAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/MouseCursorAssetManager.hpp

  src/types/assets/TextStringAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TextStringAssetManager.hpp

  src/types/assets/TerrainAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TerrainAssetManager.hpp

  src/types/assets/TextureAssetManager.cpp
  include/${CASQADIUM_SHORT}/types/assets/TextureAssetManager.hpp


#  Audio
  src/types/audio/AudioFilterFactory.cpp
  include/${CASQADIUM_SHORT}/types/audio/AudioFilterFactory.hpp

  src/types/audio/SoloudQueue.cpp
  include/${CASQADIUM_SHORT}/types/audio/SoloudQueue.hpp


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
  include/${CASQADIUM_SHORT}/types/input/MouseInputId.hpp


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
  src/types/ui/widgets/IdSelector.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/IdSelector.hpp

  src/types/ui/widgets/ToggleButton.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/ToggleButton.hpp

  src/types/ui/widgets/StringFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/StringFilter.hpp

  src/types/ui/widgets/PackageFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/PackageFilter.hpp

  src/types/ui/widgets/RegistryFilter.cpp
  include/${CASQADIUM_SHORT}/types/ui/widgets/RegistryFilter.hpp


#  Components
  src/components/audio/Audio3dParams.cpp
  src/components/audio/Audio3dParams_ui.cpp
  include/${CASQADIUM_SHORT}/components/audio/Audio3dParams.hpp

  src/components/audio/AudioBus.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioBus.hpp

  src/components/audio/AudioDrivenTransform.cpp
  src/components/audio/AudioDrivenTransform_ui.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioDrivenTransform.hpp

  src/components/audio/AudioGroupMap.cpp
  src/components/audio/AudioGroupMap_ui.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioGroupMap.hpp

  src/components/audio/AudioListener3d.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioListener3d.hpp

  src/components/audio/AudioLoop.cpp
  src/components/audio/AudioLoop_ui.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioLoop.hpp

  src/components/audio/AudioSequence.cpp
  include/${CASQADIUM_SHORT}/components/audio/AudioSequence.hpp


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

  src/components/DrawableMesh.cpp
  src/components/DrawableMesh_ui.cpp
  include/${CASQADIUM_SHORT}/components/DrawableMesh.hpp

  src/components/EntityList.cpp
  src/components/EntityList_ui.cpp
  include/${CASQADIUM_SHORT}/components/EntityList.hpp

  src/components/EntityMetaInfo.cpp
  src/components/EntityMetaInfo_ui.cpp
  include/${CASQADIUM_SHORT}/components/EntityMetaInfo.hpp

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

  src/components/SkinPool.cpp
  src/components/SkinPool_ui.cpp
  include/${CASQADIUM_SHORT}/components/SkinPool.hpp

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

  src/components/TextureTint.cpp
  src/components/TextureTint_ui.cpp
  include/${CASQADIUM_SHORT}/components/TextureTint.hpp

  src/components/Transform.cpp
  src/components/Transform_ui.cpp
  include/${CASQADIUM_SHORT}/components/Transform.hpp

  src/components/WantsMouseGrabbed.cpp
  include/${CASQADIUM_SHORT}/components/WantsMouseGrabbed.hpp

  src/components/WantsCursorOverridden.cpp
  src/components/WantsCursorOverridden_ui.cpp
  include/${CASQADIUM_SHORT}/components/WantsCursorOverridden.hpp


  src/components/assets/AudioAssetList.cpp
  src/components/assets/AudioAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/AudioAssetList.hpp

  src/components/assets/FontAssetList.cpp
  src/components/assets/FontAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/FontAssetList.hpp

  src/components/assets/MeshAssetList.cpp
  src/components/assets/MeshAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/MeshAssetList.hpp

  src/components/assets/MouseCursorAssetList.cpp
  src/components/assets/MouseCursorAssetList_ui.cpp
  include/${CASQADIUM_SHORT}/components/assets/MouseCursorAssetList.hpp

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
  src/callbacks/audio.cpp
  include/${CASQADIUM_SHORT}/callbacks/audio.hpp

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
  src/systems/audio.cpp
  include/${CASQADIUM_SHORT}/systems/audio.hpp

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

  if(${CQDE_BUILD_EXAMPLE_APP})
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE INTERNAL "" FORCE)
    set_target_properties(${TARGET} PROPERTIES
      MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
  endif()

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

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
    set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)

    if(${CQDE_BUILD_EXAMPLE_APP})
      set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "" FORCE)
      set_target_properties(${TARGET} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
      )
    endif()
  endif()
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
else()
  set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
endif()

set(ASSIMP_DOUBLE_PRECISION OFF CACHE BOOL "" FORCE)
set(ASSIMP_IGNORE_GIT_HASH ON CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
else()
  set(ASSIMP_WARNINGS_AS_ERRORS ON CACHE BOOL "" FORCE)
endif()

set(BUILD_OBJECT_LIBS OFF CACHE BOOL "" FORCE) # JsonCPP
set(JSONCPP_WITH_CMAKE_PACKAGE OFF CACHE BOOL "" FORCE)
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

if (UNIX AND APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(SOLOUD_BACKEND_MINIAUDIO ON CACHE BOOL "" FORCE)
  set(SOLOUD_BACKEND_COREAUDIO OFF CACHE BOOL "" FORCE)
endif()


include(FetchContent)

FetchContent_Declare(ctpl
  GIT_REPOSITORY https://github.com/Casqade/CTPL
  GIT_TAG        master
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(ctpl)


FetchContent_Declare(TimeUtils
  GIT_REPOSITORY https://github.com/Casqade/TimeUtils
  GIT_TAG        main
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(TimeUtils)


FetchContent_Declare(EnTT
  GIT_REPOSITORY https://github.com/skypjack/entt
  GIT_TAG        v3.11.1
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(EnTT)


FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG        3.3.8
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glfw)


FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm
  GIT_TAG        0.9.9.8
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)


FetchContent_Declare(assimp
  GIT_REPOSITORY https://github.com/assimp/assimp
  GIT_TAG        v5.3.1
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(assimp)


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


if (NOT ${CQDE_DISABLE_TBB})
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


if(CMAKE_BUILD_TYPE MATCHES "Release")
  target_compile_definitions(EnTT INTERFACE
    ENTT_DISABLE_ASSERT
  )
endif()

set(GLM_LIB_TYPE glm_static)
set(JSONCPP_LIB_TYPE jsoncpp_static)

if(${BUILD_SHARED_LIBS})
  set(GLM_LIB_TYPE glm_shared)
  set(JSONCPP_LIB_TYPE jsoncpp_lib)

else()
  target_link_libraries(${TARGET} PRIVATE 
    -static-libgcc 
    -static-libstdc++
  )
endif()

target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
  GLM_FORCE_DEPTH_ZERO_TO_ONE
)

if(${GLM_DISABLE_SIMD})
  target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
    GLM_FORCE_PURE
  )
else()
  if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${GLM_LIB_TYPE} PUBLIC
      -msse4.1
    )
  endif()

  target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
    GLM_FORCE_INTRINSICS
    GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
  )
endif()

if(${GLM_FORCE_INLINE})
  target_compile_definitions(${GLM_LIB_TYPE} PUBLIC
    GLM_FORCE_INLINE
  )
endif()

if(${SOLOUD_DISABLE_SIMD})
  target_compile_definitions(soloud PUBLIC
    DISABLE_SIMD
  )
endif()

if(${CQDE_DISABLE_TBB})
  target_compile_definitions(${TARGET} PRIVATE
    _PSTL_PAR_BACKEND_SERIAL
  )

else()

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

target_compile_definitions(${TARGET} PUBLIC
  NOMINMAX
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set_target_properties(${TARGET} PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
  )

  set_target_properties(${GLM_LIB_TYPE} PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
  )

  set_target_properties(soloud PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS TRUE
  )
endif()

set(IMGUI_BUILD_BACKEND_GLFW ON CACHE BOOL "" FORCE)
set(IMGUI_BUILD_BACKEND_OPENGL_3 ON CACHE BOOL "" FORCE)

set(IMGUI_ENABLE_OPENGL_DEBUG ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_DEBUG_TOOLS ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_DEMO_WINDOWS ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_OBSOLETE_FUNCTIONS ON CACHE BOOL "" FORCE)
set(IMGUI_DISABLE_OBSOLETE_KEYIO ON CACHE BOOL "" FORCE)

set(OLC_USE_STB_IMAGE ON CACHE BOOL "" FORCE)

add_subdirectory(deps)

find_package(Threads REQUIRED)

target_link_libraries(${TARGET} PUBLIC
  glad
  Threads::Threads
  glfw
  imgui
  imgui_backend_glfw
  imgui_backend_opengl3
  ImGuizmo
  ctpl::ctpl
  TimeUtils::TimeUtils
  assimp::assimp
  olcPGE
  EnTT::EnTT
  glm::${GLM_LIB_TYPE}
  ${JSONCPP_LIB_TYPE}
  spdlog::spdlog
  ReactPhysics3D::reactphysics3d
  soloud
)

include(GNUInstallDirs)

if(${CQDE_BUILD_EXAMPLE_APP})
  set(CQDE_APPNAME CasqadiumApp)

  add_executable(${CQDE_APPNAME})
  target_sources(${CQDE_APPNAME} PRIVATE
    example/main.cpp
    example/icon.rc
  )

  if(CMAKE_BUILD_TYPE MATCHES "Release")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_link_options(${CQDE_APPNAME} PUBLIC "/ENTRY:mainCRTStartup")
    else()
      target_compile_options(${CQDE_APPNAME} PUBLIC -O2)
    endif()

    set_target_properties(${CQDE_APPNAME} PROPERTIES
      WIN32_EXECUTABLE TRUE
    )

  else()

    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
      target_compile_options(${CQDE_APPNAME} PUBLIC -Og)
    endif()

    set_target_properties(${CQDE_APPNAME} PROPERTIES
      WIN32_EXECUTABLE FALSE
    )
  endif()

  if(NOT ${BUILD_SHARED_LIBS})
    target_link_libraries(${CQDE_APPNAME} PUBLIC -static)
  endif()

  target_link_libraries(${CQDE_APPNAME} PRIVATE
    casqadium::casqadium
  )

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
  file(CREATE_LINK "${CMAKE_CURRENT_LIST_DIR}/example/data" "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/data" SYMBOLIC)
endif()


include(GenerateExportHeader)
generate_export_header(${TARGET} EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/${CASQADIUM_SHORT}/${CASQADIUM_SHORT}_export.hpp)

target_include_directories(${TARGET} PUBLIC
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

