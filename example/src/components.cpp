#include <demo/components.hpp>

#include <cqde/json_helpers.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>

#include <spdlog/fmt/bundled/format.h>

#include <imgui.h>
#include <imgui_stdlib.h>


namespace demo
{

const static Json::Value musicalInstrumentJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& audioId = root["audioId"];
  audioId = ValueType::stringValue;
  audioId.setComment("// 'audioId' must be a JSON string"s,
                      Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
MusicalInstrument::serialize() const
{
  Json::Value json {};

  json["audioId"] = audioId.str();

  return json;
}

void
MusicalInstrument::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using cqde::jsonValidateObject;

  jsonValidateObject(json, musicalInstrumentJsonReference);

  auto& comp = registry.emplace_or_replace <MusicalInstrument> (entity);

  comp.audioId = json["audioId"].asString();
}

void
MusicalInstrument::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using cqde::ui::StringFilter;
  using cqde::types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Audio ID", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static StringFilter audioFilter {"Audio ID"};

  const auto audioList = registry.ctx().get <AudioAssetManager> ().assetIdList();

  if ( ImGui::BeginCombo("##audioId", audioId.str().c_str()) )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    audioFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto& audio : audioList )
    {
      if ( audioFilter.query(audio.str()) == false )
        continue;

      const bool selected = (audioId == audio);

      if ( ImGui::Selectable(audio.str().c_str(), selected) )
        audioId = audio;
    }
    ImGui::EndCombo();
  }
}


Json::Value
MusicConcertController::serialize() const
{
  return Json::ValueType::objectValue;
}

void
MusicConcertController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <MusicConcertController> (entity);
}


Json::Value
FootstepAudioBank::serialize() const
{
  return Json::ValueType::objectValue;
}

void
FootstepAudioBank::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  registry.emplace_or_replace <FootstepAudioBank> (entity);
}


const static Json::Value physicsMaterialJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& materialId = root["materialId"];
  materialId = ValueType::stringValue;
  materialId.setComment("// 'materialId' must be a JSON string"s,
                        Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
PhysicsMaterial::serialize() const
{
  Json::Value json {};

  json["materialId"] = materialId.str();

  return json;
}

void
PhysicsMaterial::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using cqde::jsonValidateObject;

  jsonValidateObject(json, physicsMaterialJsonReference);

  auto& comp = registry.emplace_or_replace <PhysicsMaterial> (entity);

  comp.materialId = json["materialId"].asString();
}

void
PhysicsMaterial::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using cqde::ui::StringFilter;
  using cqde::types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    std::string material = materialId.str();

    ImGui::InputText("##materialId", &material);
    materialId = material;
  }
}

const static Json::Value engineControllerJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& cylinderAudioId = root["cylinderAudioId"];
  cylinderAudioId = ValueType::stringValue;
  cylinderAudioId.setComment("// 'cylinderAudioId' must be a JSON string"s,
                              Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
EngineController::serialize() const
{
  Json::Value json {};

  json["cylinderAudioId"] = cylinderAudioId.str();

  return json;
}

void
EngineController::deserialize(
  entt::registry& registry,
  entt::entity entity,
  const Json::Value& json,
  const std::unordered_map <EntityId, EntityId,
                            identifier_hash>& idMap )
{
  using cqde::jsonValidateObject;

  jsonValidateObject(json, engineControllerJsonReference);

  auto& comp = registry.emplace_or_replace <EngineController> (entity);

  comp.cylinderAudioId = json["cylinderAudioId"].asString();
}

void
EngineController::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using cqde::ui::StringFilter;
  using cqde::types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Cylinder audio id", ImGuiTreeNodeFlags_DefaultOpen) == false )
    return;

  static StringFilter audioFilter {"Audio ID"};

  const auto assets = registry.ctx().get <AudioAssetManager> ().assetIdList();

  if ( ImGui::BeginCombo("##cylinderAudioId", cylinderAudioId.str().c_str()) )
  {
    if ( ImGui::IsWindowAppearing() )
      ImGui::SetKeyboardFocusHere(2);

    audioFilter.search({}, ImGuiInputTextFlags_AutoSelectAll);

    for ( const auto& asset : assets )
    {
      if ( audioFilter.query(asset.str()) == false )
        continue;

      const bool selected = (cylinderAudioId == asset);

      if ( ImGui::Selectable(asset.str().c_str(), selected) )
        cylinderAudioId = asset;
    }
    ImGui::EndCombo();
  }
}

} // namespace demo
