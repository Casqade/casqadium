#include <cqde/types/input/InputManager.hpp>

#include <cqde/common.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/util/logger.hpp>

#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>

#include <cqde/types/CallbackManager.hpp>

#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <olcPGE/olcMouseInputId.hpp>

#include <entt/entity/registry.hpp>

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

  mHwControlMap =
  {
    {Key::NONE, "Key_Unknown"},
    {Key::A, "Key_A"},
    {Key::D, "Key_D"},
    {Key::S, "Key_S"},
    {Key::W, "Key_W"},
    {Key::C, "Key_C"},
    {Key::Q, "Key_Q"},
    {Key::MENU, "Key_Alt"},
    {Key::CTRL, "Key_Ctrl"},
    {Key::SHIFT, "Key_Shift"},
    {Key::SPACE, "Key_Space"},
    {Key::ENTER, "Key_Enter"},
    {Key::ESCAPE, "Key_Escape"},

    {Key::LEFT, "Key_Left"},
    {Key::RIGHT, "Key_Right"},
    {Key::UP, "Key_Up"},
    {Key::DOWN, "Key_Down"},

    {InputHwCode(MouseInputId::ButtonLeft), "MouseButton_Left"},
    {InputHwCode(MouseInputId::ButtonRight), "MouseButton_Right"},
    {InputHwCode(MouseInputId::ButtonMiddle), "MouseButton_Middle"},
    {InputHwCode(MouseInputId::ButtonX1), "MouseButton_X1"},
    {InputHwCode(MouseInputId::ButtonX2), "MouseButton_X2"},

    {InputHwCode(MouseInputId::Wheel), "MouseWheel_Y"},

    {InputHwCode(MouseInputId::MoveX), "MouseMove_X"},
    {InputHwCode(MouseInputId::MoveY), "MouseMove_Y"},
    {InputHwCode(MouseInputId::PosX), "MousePos_X"},
    {InputHwCode(MouseInputId::PosY), "MousePos_Y"},

    {InputHwCode(MouseInputId::ENUM_END), "Undefined"},
  };
}

void
InputManager::load(
  const path& configPath )
{
  using fmt::format;

  if ( fileExists(configPath) == false )
    return;

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
    Validate(inputConfig);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to validate input config '{}': {}",
              configPath.string(), e.what()));
  }

  try
  {
    deserialize(inputConfig);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(
      format("Failed to load input config '{}': {}",
              configPath.string(), e.what()));
  }
}

void
InputManager::save(
  const path& configPath ) const
{
  auto inputConfig = serialize();

  LOG_TRACE("Writing input config to '{}'",
            configPath.string());

  const auto streamFlags = std::ios::out |
                           std::ios::trunc |
                           std::ios::binary;

  auto fileStream = fileOpen(configPath, streamFlags);
  fileStream << Json::writeString(jsonWriter(), inputConfig);
}

void
InputManager::deserialize(
  const Json::Value& inputConfig )
{
  LOG_DEBUG("Deserializing input config");

  for ( const auto& axisId : inputConfig.getMemberNames() )
  {
    LOG_DEBUG("Deserializing axis '{}'", axisId);

    for ( const auto& bindingHwId : inputConfig[axisId].getMemberNames() )
    {
      LOG_TRACE("Deserializing axis '{}' binding '{}'",
                axisId, bindingHwId);

      try
      {
        const auto& bindingJson = inputConfig[axisId][bindingHwId];

        auto binding = InputBinding::FromJson(bindingHwId,
                                              bindingJson);

        assignBinding(axisId, std::move(binding));
      }
      catch ( const std::exception& e )
      {
        using fmt::format;
        throw std::runtime_error(
          format("Failed to deserialize axis '{}' binding '{}' - {}",
                  axisId, bindingHwId, e.what()));
      }
    }
  }
}

Json::Value
InputManager::serialize() const
{
  Json::Value inputConfig {};

  LOG_DEBUG("Serializing input config");

  for ( const auto& [binding, axisId] : mBindings )
    inputConfig[axisId.str()][binding->inputId.str()] = binding->toJson();

  return inputConfig;
}

void
InputManager::clear()
{
  mBindings.clear();
  mInputHistory.clear();
}

void
InputManager::Validate(
  const Json::Value& inputConfig )
{
  jsonValidateObject(inputConfig, inputConfigReference);
}

void InputManager::assignBinding(
  const InputAxisId& axisId,
  const std::shared_ptr <InputBinding> binding )
{
  if ( bindingAssigned(axisId, binding->inputId) == false )
  {
    LOG_TRACE("Binding input '{}' to axis '{}'",
              binding->inputId.str(), axisId.str());

    mBindings.insert({ binding, axisId });
  }
}

void InputManager::assignBindings(
  const InputAxisId& axisId,
  const InputBindings& bindings )
{
  for ( auto& binding : bindings )
    assignBinding(axisId, binding);
}

void
InputManager::handleAxisInput(
  const InputHwCode inputHwCode,
  const float amount,
  const int8_t direction,
  entt::registry& registry )
{
  using compos::InputController;
  using compos::SubscriberInput;

  if ( mHwControlMap.count(inputHwCode) == 0 )
    return;

  const std::string prefix  = direction > 0
                              ? "+"
                              : direction < 0
                                ? "-" : "~";

  InputHwId inputId = prefix + mHwControlMap[inputHwCode].str();

  InputEvent event {};

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

    for ( auto&& [entity, cController] : registry.view <InputController, SubscriberInput> ().each() )
      if ( const auto& iter = cController.axes.find(axisId);
           iter != cController.axes.end() )
      {
        auto& [id, axis] = *iter;

        binding->handleInput(axis, amount);

        for ( const auto& callbackId : axis.callbacks )
          callbackMgr.execute(callbackId, registry,
                              {entity, &cController});
      }
  }
}

bool
InputManager::axisAssigned(
  const InputAxisId& axisId ) const
{
  for ( const auto& [binding, axis] : mBindings )
    if ( axis == axisId )
      return true;

  return false;
}

bool
InputManager::bindingAssigned(
  const InputAxisId& axisId,
  const InputHwId binding ) const
{
  const auto [axesBegin, axesEnd] = mBindings.equal_range(binding);

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
    if ( auto& [binding, axis] = *iter; axis == axisId )
      return true;

  return false;
}

std::set <InputAxisId>
InputManager::axes() const
{
  std::set <InputHwId> axes {};

  for ( auto& [binding, axis] : mBindings )
    axes.insert(axis);

  return axes;
}

std::vector <InputAxisId>
InputManager::axes(
  const InputHwId binding ) const
{
  std::vector <InputAxisId> axes {};

  const auto [axesBegin, axesEnd] = mBindings.equal_range(binding);

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
  {
    auto& [binding, axis] = *iter;
    axes.push_back(axis);
  }

  return axes;
}

std::vector <InputHwId>
InputManager::bindings(
  const InputAxisId& axisId ) const
{
  std::vector <InputHwId> bindings {};

  for ( auto& [binding, axis] : mBindings )
    if ( axis == axisId )
      bindings.push_back(binding->inputId);

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

const std::deque <InputEvent>&
InputManager::inputHistory() const
{
  return mInputHistory;
}

} // namespace cqde::types
