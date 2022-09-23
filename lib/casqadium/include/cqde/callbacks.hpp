#pragma once

#include <entt/fwd.hpp>

#include <any>
#include <vector>


namespace cqde::callbacks
{

void mouseAutoCenterEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseAutoCenterDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseAutoCenterToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseCursorHide(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseCursorShow(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void mouseGrabToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorModeEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorModeDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorModeToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetModeTranslate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetModeRotate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetModeScale(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetSpaceLocal(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorGizmoSetSpaceWorld(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntityMultipleSelectionEnable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntityMultipleSelectionDisable(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntityMultipleSelectionToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraCreate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorControllerCreate(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorBindingsAssign(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraControlOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraControlOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraFovControl(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraZoomControl(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorEntitySelect(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickSave(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void quickLoad(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityUpdateToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputOn(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputOff(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void entityInputToggle(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraTranslateXRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraTranslateYRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void editorCameraTranslateZRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlTranslateXRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlTranslateYRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlTranslateZRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlPitchRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlYawRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void controlRollRelative(
  entt::registry&,
  const std::vector <std::any>& args = {} );


void loadAudioAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadFontAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadGeometryAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadTextStringAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void loadTextureAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadAudioAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadFontAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadGeometryAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadTextStringAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

void unloadTextureAssets(
  entt::registry&,
  const std::vector <std::any>& args = {} );

} // cqde::callbacks
