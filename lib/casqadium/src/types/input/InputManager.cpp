#include <cqde/types/input/InputManager.hpp>

#include <cqde/common.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/InputController.hpp>

#include <cqde/types/CallbackManager.hpp>

#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <entt/entity/registry.hpp>

#include <spdlog/fmt/bundled/format.h>

#include <json/value.h>
#include <json/writer.h>


namespace cqde::types
{

const static Json::Value inputConfigReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// input config JSON root must be an object"s,
                       Json::CommentPlacement::commentBefore);

  Json::Value& axis = reference["cqde_json_anykey"];
  axis = ValueType::objectValue;
  axis.setComment("// input config axis entry must be a JSON object"s,
                  Json::CommentPlacement::commentBefore);

  Json::Value& bindings = axis["cqde_json_anykey"];
  bindings = ValueType::objectValue;
  bindings.setComment("// input config axis binding entry must be a JSON object"s,
                      Json::CommentPlacement::commentBefore);

  return reference;
}();


InputManager::InputManager()
{
  using olc::Key;
  using olc::MouseInputId;
  using cqde::InputHwId;

  mHwControlMap =
  {
    {Key::NONE, "Key_Unknown"_id},
    {Key::A, "Key_A"_id},
    {Key::D, "Key_D"_id},
    {Key::S, "Key_S"_id},
    {Key::W, "Key_W"_id},
    {Key::C, "Key_C"_id},
    {Key::MENU, "Key_Alt"_id},
    {Key::SHIFT, "Key_Shift"_id},
    {Key::SPACE, "Key_Space"_id},
    {Key::ENTER, "Key_Enter"_id},

    {InputHwCode(MouseInputId::ButtonLeft), "MouseButton_Left"_id},
    {InputHwCode(MouseInputId::ButtonRight), "MouseButton_Right"_id},
    {InputHwCode(MouseInputId::ButtonMiddle), "MouseButton_Middle"_id},
    {InputHwCode(MouseInputId::ButtonX1), "MouseButton_X1"_id},
    {InputHwCode(MouseInputId::ButtonX2), "MouseButton_X2"_id},

    {InputHwCode(MouseInputId::Wheel), "MouseWheel_Y"_id},

    {InputHwCode(MouseInputId::MoveX), "MouseMove_X"_id},
    {InputHwCode(MouseInputId::MoveY), "MouseMove_Y"_id},
    {InputHwCode(MouseInputId::PosX), "MousePos_X"_id},
    {InputHwCode(MouseInputId::PosY), "MousePos_Y"_id},

    {InputHwCode(MouseInputId::ENUM_END), "Undefined"_id},
  };
}

void
InputManager::load( const path& configPath )
{
  using fmt::format;

  Json::Value inputConfig {};

  LOG_TRACE("Parsing input config '{}'",
            configPath.string());

  try
  {
    inputConfig = fileParse(configPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to parse input config ({})",
              e.what()));
  }

  LOG_TRACE("Validating input config '{}'",
            configPath.string());

  try
  {
    jsonValidateObject(inputConfig, inputConfigReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate input config '{}': {}",
              configPath.string(), e.what()));
  }

  for ( const auto& axisId : inputConfig.getMemberNames() )
  {
    LOG_DEBUG("Binding inputs to axis '{}' ('{}')",
              axisId, configPath.string());

    for ( const auto& bindingHwId : inputConfig[axisId].getMemberNames() )
    {
      LOG_TRACE("Binding input '{}' to axis '{}' ('{}')",
                bindingHwId, axisId, configPath.string());

      try
      {
        const auto& bindingJson = inputConfig[axisId][bindingHwId];

        auto binding = InputBinding::FromJson(bindingHwId,
                                              bindingJson);

        assignBinding(axisId, std::move(binding));
      }
      catch ( const std::exception& e )
      {
        throw std::runtime_error(
          format("Failed to bind '{}' to axis '{}' ('{}'): {}",
                  bindingHwId, axisId,
                  configPath.string(), e.what()));
      }
    }
  }
}

void
InputManager::save( const path& configPath ) const
{
  Json::Value inputConfig {};

  LOG_DEBUG("Serializing input config");

  for ( const auto& [binding, axisId] : mBindings )
    inputConfig[axisId.str()][binding->inputId.str()] = binding->toJson();

  LOG_TRACE("Writing input config to '{}'",
            configPath.string());

  auto fileStream = fileOpen(configPath, std::ios::out | std::ios::trunc);
  fileStream << Json::writeString(jsonWriter(), inputConfig);
}

void InputManager::assignBinding(
  const InputAxisId axisId,
  const std::shared_ptr <InputBinding> binding )
{
  mBindings.insert({ binding, axisId });
}

void InputManager::assignBindings(
  const InputAxisId axisId,
  const InputBindings& bindings )
{
  for ( auto& binding : bindings )
    mBindings.insert({ binding, axisId });
}

void
InputManager::handleAxisInput(
  const InputHwCode inputHwCode,
  const float amount,
  const float direction,
  entt::registry& registry )
{
  using compos::InputController;

  InputEvent event {};

  const std::string inputDir  = direction > 0.0f
                              ? "+"
                              : "-";

  InputHwId inputId = inputDir + mHwControlMap[inputHwCode].str();

  event.inputId = inputId;
  event.amount = amount;

  mInputHistory.push_back(event);
  while ( mInputHistory.size() > mInputHistoryLength )
    mInputHistory.pop_front();

  auto& callbackMgr = registry.ctx().at <CallbackManager> ();

  const auto [axesBegin, axesEnd] = mBindings.equal_range(inputId);

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
  {
    auto& [binding, axisId] = *iter;

    for ( auto&& [entity, cController] : registry.view <InputController> ().each() )
      if ( const auto& iter = cController.inputs.find(axisId);
           iter != cController.inputs.end() )
      {
        auto& [id, axis] = *iter;

        binding->handleInput(axis, amount);

        for ( const auto& callbackId : axis.callbacks )
          callbackMgr.execute(callbackId, registry,
                              {entity, &cController});
      }
  }
}

std::set <InputHwId>
InputManager::toHwId(
  const InputAxisId& axisId ) const
{
  std::set <InputHwId> bindings {};

  for ( auto& [binding, axis] : mBindings )
    if ( axis == axisId )
      bindings.insert(binding->inputId);

  return bindings;
}

void
InputManager::setInputHistoryLength(
const size_t length )
{
  mInputHistoryLength = length;
}

size_t
InputManager::inputHistoryLength() const
{
  return mInputHistoryLength;
}

const std::list <InputEvent>&
InputManager::inputHistory() const
{
  return mInputHistory;
}

} // namespace cqde::types
