#include <cqde/types/CasqadiumEngine.hpp>

#include <cqde/callbacks.hpp>

#include <cqde/types/CallbackManager.hpp>


namespace cqde::types
{

void
CasqadiumEngine::registerCallbacks()
{
  using namespace cqde::callbacks;

  auto& callbackManager = mRegistry.ctx().get <CallbackManager> ();

  callbackManager.Register("EditorModeEnable", editorModeEnable);
  callbackManager.Register("EditorModeDisable", editorModeDisable);
  callbackManager.Register("EditorModeToggle", editorModeToggle);

  callbackManager.Register("EditorGizmoSetModeTranslate", editorGizmoSetModeTranslate);
  callbackManager.Register("EditorGizmoSetModeRotate", editorGizmoSetModeRotate);
  callbackManager.Register("EditorGizmoSetModeScale", editorGizmoSetModeScale);

  callbackManager.Register("EditorGizmoSetSpaceLocal", editorGizmoSetSpaceLocal);
  callbackManager.Register("EditorGizmoSetSpaceWorld", editorGizmoSetSpaceWorld);

  callbackManager.Register("EditorEntityMultipleSelectionEnable", editorEntityMultipleSelectionEnable);
  callbackManager.Register("EditorEntityMultipleSelectionDisable", editorEntityMultipleSelectionDisable);
  callbackManager.Register("EditorEntityMultipleSelectionToggle", editorEntityMultipleSelectionToggle);

  callbackManager.Register("EditorCameraCreate", editorCameraCreate);
  callbackManager.Register("EditorControllerCreate", editorControllerCreate);
  callbackManager.Register("EditorBindingsAssign", editorBindingsAssign);

  callbackManager.Register("EditorCameraSpeedControl", editorCameraSpeedControl);

  callbackManager.Register("EditorCameraTranslateXRelative", editorCameraTranslateXRelative);
  callbackManager.Register("EditorCameraTranslateYRelative", editorCameraTranslateYRelative);
  callbackManager.Register("EditorCameraTranslateZRelative", editorCameraTranslateZRelative);

  callbackManager.Register("EditorCameraControlOn", editorCameraControlOn);
  callbackManager.Register("EditorCameraControlOff", editorCameraControlOff);
  callbackManager.Register("EditorCameraFovControl", editorCameraFovControl);
  callbackManager.Register("EditorCameraZoomControl", editorCameraZoomControl);
  callbackManager.Register("EditorEntitySelect", editorEntitySelect);

  callbackManager.Register("QuickSave", quickSave);
  callbackManager.Register("QuickLoad", quickLoad);

  callbackManager.Register("EngineShutdown", engineShutdown);

  callbackManager.Register("Interact", interact);
  callbackManager.Register("InteractProbe", interactProbe);

  callbackManager.Register("EntityRemove", entityRemove);

  callbackManager.Register("EntityUpdateOn", entityUpdateOn);
  callbackManager.Register("EntityUpdateOff", entityUpdateOff);
  callbackManager.Register("EntityUpdateToggle", entityUpdateToggle);
  callbackManager.Register("EntitiesUpdateOn", entitiesUpdateOn);
  callbackManager.Register("EntitiesUpdateOff", entitiesUpdateOff);
  callbackManager.Register("EntitiesUpdateToggle", entitiesUpdateToggle);

  callbackManager.Register("EntityInputOn", entityInputOn);
  callbackManager.Register("EntityInputOff", entityInputOff);
  callbackManager.Register("EntityInputToggle", entityInputToggle);
  callbackManager.Register("EntitiesInputOn", entitiesInputOn);
  callbackManager.Register("EntitiesInputOff", entitiesInputOff);
  callbackManager.Register("EntitiesInputToggle", entitiesInputToggle);

  callbackManager.Register("SystemsActivate", systemsActivate);
  callbackManager.Register("SystemsDeactivate", systemsDeactivate);
  callbackManager.Register("SystemsToggle", systemsToggle);

  callbackManager.Register("LoadAudioAssets", loadAudioAssets);
  callbackManager.Register("LoadFontAssets", loadFontAssets);
  callbackManager.Register("LoadMeshAssets", loadMeshAssets);
  callbackManager.Register("LoadTerrainAssets", loadTerrainAssets);
  callbackManager.Register("LoadTextStringAssets", loadTextStringAssets);
  callbackManager.Register("LoadTextureAssets", loadTextureAssets);

  callbackManager.Register("LoadAllAudioAssets", loadAllAudioAssets);
  callbackManager.Register("LoadAllFontAssets", loadAllFontAssets);
  callbackManager.Register("LoadAllMeshAssets", loadAllMeshAssets);
  callbackManager.Register("LoadAllTerrainAssets", loadAllTerrainAssets);
  callbackManager.Register("LoadAllTextStringAssets", loadAllTextStringAssets);
  callbackManager.Register("LoadAllTextureAssets", loadAllTextureAssets);

  callbackManager.Register("UnloadAudioAssets", unloadAudioAssets);
  callbackManager.Register("UnloadFontAssets", unloadFontAssets);
  callbackManager.Register("UnloadMeshAssets", unloadMeshAssets);
  callbackManager.Register("UnloadTerrainAssets", unloadTerrainAssets);
  callbackManager.Register("UnloadTextStringAssets", unloadTextStringAssets);
  callbackManager.Register("UnloadTextureAssets", unloadTextureAssets);

  callbackManager.Register("WaitAudioAssetsLoaded", waitAudioAssetsLoaded);
  callbackManager.Register("WaitFontAssetsLoaded", waitFontAssetsLoaded);
  callbackManager.Register("WaitMeshAssetsLoaded", waitMeshAssetsLoaded);
  callbackManager.Register("WaitTerrainAssetsLoaded", waitTerrainAssetsLoaded);
  callbackManager.Register("WaitTextStringAssetsLoaded", waitTextStringAssetsLoaded);
  callbackManager.Register("WaitTextureAssetsLoaded", waitTextureAssetsLoaded);

  callbackManager.Register("WaitAllAudioAssetsLoaded", waitAllAudioAssetsLoaded);
  callbackManager.Register("WaitAllFontAssetsLoaded", waitAllFontAssetsLoaded);
  callbackManager.Register("WaitAllMeshAssetsLoaded", waitAllMeshAssetsLoaded);
  callbackManager.Register("WaitAllTerrainAssetsLoaded", waitAllTerrainAssetsLoaded);
  callbackManager.Register("WaitAllTextStringAssetsLoaded", waitAllTextStringAssetsLoaded);
  callbackManager.Register("WaitAllTextureAssetsLoaded", waitAllTextureAssetsLoaded);

  callbackManager.Register("ControlTranslateXRelative", controlTranslateXRelative);
  callbackManager.Register("ControlTranslateYRelative", controlTranslateYRelative);
  callbackManager.Register("ControlTranslateZRelative", controlTranslateZRelative);

  callbackManager.Register("ControlPitchRelative", controlPitchRelative);
  callbackManager.Register("ControlYawRelative", controlYawRelative);
  callbackManager.Register("ControlRollRelative", controlRollRelative);

  callbackManager.Register("ForceEmitterCallback", forceEmitterCallback);
  callbackManager.Register("GravityEmitterCallback", gravityEmitterCallback);

  callbackManager.Register("ForceXRelative", forceXRelative);
  callbackManager.Register("ForceYRelative", forceYRelative);
  callbackManager.Register("ForceZRelative", forceZRelative);

  callbackManager.Register("TorqueXRelative", torqueXRelative);
  callbackManager.Register("TorqueYRelative", torqueYRelative);
  callbackManager.Register("TorqueZRelative", torqueZRelative);

  callbackManager.Register("SpeedLinearXRelative", speedLinearXRelative);
  callbackManager.Register("SpeedLinearYRelative", speedLinearYRelative);
  callbackManager.Register("SpeedLinearZRelative", speedLinearZRelative);

  callbackManager.Register("SpeedLinearLocalXRelative", speedLinearLocalXRelative);
  callbackManager.Register("SpeedLinearLocalYRelative", speedLinearLocalYRelative);
  callbackManager.Register("SpeedLinearLocalZRelative", speedLinearLocalZRelative);

  callbackManager.Register("SpeedAngularXRelative", speedAngularXRelative);
  callbackManager.Register("SpeedAngularYRelative", speedAngularYRelative);
  callbackManager.Register("SpeedAngularZRelative", speedAngularZRelative);
}

} // namespace cqde::types
