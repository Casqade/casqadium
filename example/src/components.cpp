#include <demo/components.hpp>

#include <cqde/logger.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/types/assets/AudioAssetManager.hpp>
#include <cqde/types/ui/widgets/StringFilter.hpp>

#include <entt/entity/registry.hpp>

#include <glm/gtc/constants.hpp>

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


void
CarEngine::update(
  const float throttle,
  const float clutch,
  const float dt )
{
//  const float loadTorque = engineLoad * gearRatios[currentGear];
  const float loadTorque = 0.0f;
  const float targetTorque = torque.max * throttle * clutch * torqueCurve() - loadTorque;
  float targetRPM = rpm.max * targetTorque / torque.max;
  targetRPM = std::clamp(targetRPM, rpm.idle, rpm.redline);

  rpm.current += (targetRPM - rpm.current) * dt * rpm.sensitivity;

  const float coolingRate = temperature.coolingRate * (temperature.current - temperature.ambient) * dt;
  temperature.current += (rpm.current / rpm.redline) * dt * temperature.sensitivity - coolingRate;
  temperature.current = std::clamp(temperature.current, temperature.ambient, temperature.max);

  torque.current = torque.max * throttle * clutch * torqueCurve() * temperatureFactor();

  soundVolume.current = soundVolume.base + (rpm.current / rpm.redline) * (1.0f - soundVolume.base) * throttle;

  const float relativeRPM = (rpm.current - rpm.idle) / (rpm.redline - rpm.idle);
  soundSpeed.current = soundSpeed.min + ((std::pow(2.0f, relativeRPM) - 1.0f) * (soundSpeed.max - soundSpeed.min));

  cutoffFrequency.current = cutoffFrequency.lower + (rpm.current / rpm.redline) * (cutoffFrequency.upper - cutoffFrequency.lower);
}

float
CarEngine::torqueCurve() const
{
  return std::sin(rpm.current / rpm.redline + 0.9f);
}

float
CarEngine::temperatureFactor() const
{
  if ( temperature.current < temperature.optimal )
  {
    const float range = temperature.optimal - temperature.ambient;
    const float delta = (temperature.optimal - temperature.current) / range;

    return std::sin((delta + 0.5f) * glm::pi <float> () / 3.0f);
  }
  else
  {
    const float range = temperature.max - temperature.optimal;
    const float delta = (temperature.current - temperature.optimal) / range;

    return -std::pow(delta, 3.0f) + 1.0f;
  }
}

void
CarEngine::ui_show()
{
  const auto flags =
    ImGuiSliderFlags_NoRoundToFormat |
    ImGuiSliderFlags_AlwaysClamp;

  if ( ImGui::CollapsingHeader("Torque", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Current torque: %.1f", torque.current);

    ImGui::DragFloat( "##torqueMax", &torque.max,
      10.0f, 0.0f,
      std::numeric_limits <float>::max(),
      "Max torque: %.1f", flags);
  }

  if ( ImGui::CollapsingHeader("RPM", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Current RPM: %.1f", rpm.current);

    ImGui::DragFloat( "##rpmIdle",
      &rpm.idle,
      10.0f, 0.0f, rpm.optimal,
      "Idle RPM: %.0f", flags);

    ImGui::DragFloat( "##rpmOptimal",
      &rpm.optimal,
      10.0f, rpm.idle, rpm.redline,
      "Optimal RPM: %.0f", flags);

    ImGui::DragFloat( "##rpmRedline",
      &rpm.redline,
      10.0f, rpm.optimal, rpm.max,
      "Redline RPM: %.0f", flags);

    ImGui::DragFloat( "##rpmMax",
      &rpm.max,
      10.0f, rpm.redline,
      std::numeric_limits <float>::max(),
      "Max RPM: %.0f", flags);

    ImGui::DragFloat( "##rpmSensitivity",
      &rpm.sensitivity,
      0.1f, 0.0f,
      std::numeric_limits <float>::max(),
      "RPM sensitivity: %.2f", flags);
  }

  if ( ImGui::CollapsingHeader("Temperature", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Current Temperature: %.1f", temperature.current);

    ImGui::DragFloat( "##temperatureAmbient",
      &temperature.ambient, 1.0f,
      0.0f, temperature.optimal,
      "Ambient temperature: %.1f", flags);

    ImGui::DragFloat( "##temperatureOptimal",
      &temperature.optimal, 1.0f,
      temperature.ambient, temperature.max,
      "Optimal temperature: %.1f", flags);

    ImGui::DragFloat( "##temperatureMax",
      &temperature.max, 1.0f,
      temperature.optimal,
      std::numeric_limits <float>::max(),
      "Max temperature: %.1f", flags);

    ImGui::DragFloat( "##temperatureSensitivity",
      &temperature.sensitivity, 0.1f,
      0.0f, std::numeric_limits <float>::max(),
      "Temperature sensitivity: %.2f", flags);

    ImGui::DragFloat( "##coolingRate",
      &temperature.coolingRate, 0.1f,
      0.0f,
      std::numeric_limits <float>::max(),
      "Cooling rate: %.2f", flags);
  }

  if ( ImGui::CollapsingHeader("Sound", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Current volume: %.2f", soundVolume.current);
    ImGui::Text("Current pitch: %.2f", soundSpeed.current);
    ImGui::Text("Current cutoff: %.0f", cutoffFrequency.current);
    ImGui::Text("Current lower cutoff: %.0f", cutoffFrequency.lower);
    ImGui::Text("Current upper cutoff: %.0f", cutoffFrequency.upper);

    ImGui::DragFloat( "##soundPitchMin", &soundSpeed.min,
      0.01f, 0.0f,
      soundSpeed.max,
      "Sound pitch min: %.2f", flags);

    ImGui::DragFloat( "##soundPitchMax", &soundSpeed.max,
      0.01f, soundSpeed.min,
      std::numeric_limits <float>::max(),
      "Sound pitch max: %.2f", flags);
  }
}

const static Json::Value carEngineJsonReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value root = ValueType::objectValue;
  root.setComment("// root must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& torque = root["torque"];
  torque = ValueType::objectValue;
  torque.setComment("// 'torque' must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& torqueCurrent = torque["current"];
  torqueCurrent = ValueType::realValue;
  torqueCurrent.setComment("// 'torque.current' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);

  Json::Value& torqueMax = torque["max"];
  torqueMax = ValueType::realValue;
  torqueMax.setComment("// 'torque.max' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);


  Json::Value& rpm = root["rpm"];
  rpm = ValueType::objectValue;
  rpm.setComment("// 'rpm' must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& rpmCurrent = rpm["current"];
  rpmCurrent = ValueType::realValue;
  rpmCurrent.setComment("// 'rpm.current' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& rpmIdle = rpm["idle"];
  rpmIdle = ValueType::realValue;
  rpmIdle.setComment("// 'rpm.idle' must be a JSON float"s,
                      Json::CommentPlacement::commentBefore);

  Json::Value& rpmOptimal = rpm["optimal"];
  rpmOptimal = ValueType::realValue;
  rpmOptimal.setComment("// 'rpm.optimal' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& rpmRedline = rpm["redline"];
  rpmRedline = ValueType::realValue;
  rpmRedline.setComment("// 'rpm.redline' must be a JSON float"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& rpmMax = rpm["max"];
  rpmMax = ValueType::realValue;
  rpmMax.setComment("// 'rpm.max' must be a JSON float"s,
                    Json::CommentPlacement::commentBefore);

  Json::Value& rpmSensitivity = rpm["sensitivity"];
  rpmSensitivity = ValueType::realValue;
  rpmSensitivity.setComment("// 'rpm.sensitivity' must be a JSON float"s,
                      Json::CommentPlacement::commentBefore);


  Json::Value& temperature = root["temperature"];
  temperature = ValueType::objectValue;
  temperature.setComment("// 'temperature' must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& remperatureAmbient = temperature["ambient"];
  remperatureAmbient = ValueType::realValue;
  remperatureAmbient.setComment("// 'temperature.ambient' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  Json::Value& remperatureOptimal = temperature["optimal"];
  remperatureOptimal = ValueType::realValue;
  remperatureOptimal.setComment("// 'temperature.optimal' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  Json::Value& remperatureMax = temperature["max"];
  remperatureMax = ValueType::realValue;
  remperatureMax.setComment("// 'temperature.max' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);

  Json::Value& remperatureCurrent = temperature["current"];
  remperatureCurrent = ValueType::realValue;
  remperatureCurrent.setComment("// 'temperature.current' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  Json::Value& remperatureSensitivity = temperature["sensitivity"];
  remperatureSensitivity = ValueType::realValue;
  remperatureSensitivity.setComment("// 'temperature.sensitivity' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  Json::Value& remperatureCoolingRate = temperature["coolingRate"];
  remperatureCoolingRate = ValueType::realValue;
  remperatureCoolingRate.setComment("// 'temperature.coolingRate' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);


  Json::Value& soundVolume = root["soundVolume"];
  soundVolume = ValueType::objectValue;
  soundVolume.setComment("// 'soundVolume' must be a JSON object"s,
                          Json::CommentPlacement::commentBefore);

  Json::Value& soundVolumeCurrent = soundVolume["current"];
  soundVolumeCurrent = ValueType::realValue;
  soundVolumeCurrent.setComment("// 'soundVolume.current' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  Json::Value& soundVolumeBase = soundVolume["base"];
  soundVolumeBase = ValueType::realValue;
  soundVolumeBase.setComment("// 'soundVolume.base' must be a JSON float"s,
                              Json::CommentPlacement::commentBefore);


  Json::Value& soundSpeed = root["soundSpeed"];
  soundSpeed = ValueType::objectValue;
  soundSpeed.setComment("// 'soundSpeed' must be a JSON object"s,
                        Json::CommentPlacement::commentBefore);

  Json::Value& soundSpeedCurrent = soundSpeed["current"];
  soundSpeedCurrent = ValueType::realValue;
  soundSpeedCurrent.setComment("// 'soundSpeed.current' must be a JSON float"s,
                                Json::CommentPlacement::commentBefore);

  Json::Value& soundSpeedMin = soundSpeed["min"];
  soundSpeedMin = ValueType::realValue;
  soundSpeedMin.setComment("// 'soundSpeed.min' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);

  Json::Value& soundSpeedMax = soundSpeed["max"];
  soundSpeedMax = ValueType::realValue;
  soundSpeedMax.setComment("// 'soundSpeed.max' must be a JSON float"s,
                            Json::CommentPlacement::commentBefore);


  Json::Value& cutoffFrequency = root["cutoffFrequency"];
  cutoffFrequency = ValueType::objectValue;
  cutoffFrequency.setComment("// 'cutoffFrequency' must be a JSON object"s,
                              Json::CommentPlacement::commentBefore);

  Json::Value& cutoffFrequencyCurrent = cutoffFrequency["current"];
  cutoffFrequencyCurrent = ValueType::realValue;
  cutoffFrequencyCurrent.setComment("// 'cutoffFrequency.current' must be a JSON float"s,
                                    Json::CommentPlacement::commentBefore);

  Json::Value& cutoffFrequencyLower = cutoffFrequency["lower"];
  cutoffFrequencyLower = ValueType::realValue;
  cutoffFrequencyLower.setComment("// 'cutoffFrequency.lower' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  Json::Value& cutoffFrequencyUpper = cutoffFrequency["upper"];
  cutoffFrequencyUpper = ValueType::realValue;
  cutoffFrequencyUpper.setComment("// 'cutoffFrequency.upper' must be a JSON float"s,
                                  Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
CarEngine::serialize() const
{
  Json::Value json {};

  auto& torqueJson = json["torque"];
  torqueJson["current"] = torque.current;
  torqueJson["max"] = torque.max;

  auto& rpmJson = json["rpm"];
  rpmJson["current"] = rpm.current;
  rpmJson["idle"] = rpm.idle;
  rpmJson["optimal"] = rpm.optimal;
  rpmJson["redline"] = rpm.redline;
  rpmJson["max"] = rpm.max;
  rpmJson["sensitivity"] = rpm.sensitivity;

  auto& temperatureJson = json["temperature"];
  temperatureJson["ambient"] = temperature.ambient;
  temperatureJson["optimal"] = temperature.optimal;
  temperatureJson["max"] = temperature.max;
  temperatureJson["current"] = temperature.current;
  temperatureJson["sensitivity"] = temperature.sensitivity;
  temperatureJson["coolingRate"] = temperature.coolingRate;

  auto& soundVolumeJson = json["soundVolume"];
  soundVolumeJson["current"] = soundVolume.current;
  soundVolumeJson["base"] = soundVolume.base;

  auto& soundSpeedJson = json["soundSpeed"];
  soundSpeedJson["current"] = soundSpeed.current;
  soundSpeedJson["min"] = soundSpeed.min;
  soundSpeedJson["max"] = soundSpeed.max;

  auto& cutoffFrequencyJson = json["cutoffFrequency"];
  cutoffFrequencyJson["current"] = cutoffFrequency.current;
  cutoffFrequencyJson["lower"] = cutoffFrequency.lower;
  cutoffFrequencyJson["upper"] = cutoffFrequency.upper;

  return json;
}

void
CarEngine::deserialize(
  const Json::Value& json )
{
  cqde::jsonValidateObject(json, carEngineJsonReference);

  auto& torqueJson = json["torque"];
  torque.current = torqueJson["current"].asFloat();
  torque.max = torqueJson["max"].asFloat();

  auto& rpmJson = json["rpm"];
  rpm.current = rpmJson["current"].asFloat();
  rpm.idle = rpmJson["idle"].asFloat();
  rpm.optimal = rpmJson["optimal"].asFloat();
  rpm.redline = rpmJson["redline"].asFloat();
  rpm.max = rpmJson["max"].asFloat();
  rpm.sensitivity = rpmJson["sensitivity"].asFloat();

  auto& temperatureJson = json["temperature"];
  temperature.ambient = temperatureJson["ambient"].asFloat();
  temperature.optimal = temperatureJson["optimal"].asFloat();
  temperature.max = temperatureJson["max"].asFloat();
  temperature.current = temperatureJson["current"].asFloat();
  temperature.sensitivity = temperatureJson["sensitivity"].asFloat();
  temperature.coolingRate = temperatureJson["coolingRate"].asFloat();

  auto& soundVolumeJson = json["soundVolume"];
  soundVolume.current = soundVolumeJson["current"].asFloat();
  soundVolume.base = soundVolumeJson["base"].asFloat();

  auto& soundSpeedJson = json["soundSpeed"];
  soundSpeed.current = soundSpeedJson["current"].asFloat();
  soundSpeed.min = soundSpeedJson["min"].asFloat();
  soundSpeed.max = soundSpeedJson["max"].asFloat();

  auto& cutoffFrequencyJson = json["cutoffFrequency"];
  cutoffFrequency.current = cutoffFrequencyJson["current"].asFloat();
  cutoffFrequency.lower = cutoffFrequencyJson["lower"].asFloat();
  cutoffFrequency.upper = cutoffFrequencyJson["upper"].asFloat();
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

  Json::Value& engine = root["engine"];
  engine = ValueType::objectValue;
  engine.setComment("// 'engine' must be a JSON object"s,
                    Json::CommentPlacement::commentBefore);

  return root;
}();

Json::Value
EngineController::serialize() const
{
  Json::Value json {};

  json["cylinderAudioId"] = cylinderAudioId.str();
  json["engine"] = engine.serialize();

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
  comp.engine.deserialize(json["engine"]);
}

void
EngineController::ui_edit_props(
  const entt::entity entity,
  const entt::registry& registry)
{
  using cqde::ui::StringFilter;
  using cqde::types::AudioAssetManager;

  if ( ImGui::CollapsingHeader("Cylinder audio id", ImGuiTreeNodeFlags_DefaultOpen) )
  {
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

  if ( ImGui::CollapsingHeader("Engine input", ImGuiTreeNodeFlags_DefaultOpen) )
  {
    ImGui::Text("Throttle: %.2f", throttle);
    ImGui::Text("Clutch: %.2f", clutch);
  }

  engine.ui_show();
}

} // namespace demo
