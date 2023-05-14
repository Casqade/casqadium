#include <cqde/types/input/InputManager.hpp>

#include <cqde/file_helpers.hpp>
#include <cqde/json_helpers.hpp>

#include <cqde/logger.hpp>

#include <cqde/components/CasqadiumEditorInternal.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SubscriberInput.hpp>
#include <cqde/components/SubscriberUpdate.hpp>

#include <cqde/types/CallbackManager.hpp>

#include <cqde/types/input/MouseInputId.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>

#include <entt/entity/registry.hpp>

#include <GLFW/glfw3.h>

#include <json/value.h>
#include <json/writer.h>

#include <fstream>


namespace cqde::types
{

const static Json::Value inputConfigReference =
[]
{
  using ValueType = Json::ValueType;
  using namespace std::string_literals;

  Json::Value reference = ValueType::objectValue;
  reference.setComment("// input config root must be a JSON object"s,
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
  constexpr auto MouseEnumStart =
    static_cast <InputHwCode> (MouseInputId::ENUM_START);

  mHwControlMap =
  {
    {GLFW_KEY_UNKNOWN, "Key_Unknown"},

    {GLFW_KEY_SPACE, "Key_Space"},

    {GLFW_KEY_APOSTROPHE, "Key_Apostrophe"},
    {GLFW_KEY_COMMA, "Key_Comma"},
    {GLFW_KEY_MINUS, "Key_Minus"},
    {GLFW_KEY_PERIOD, "Key_Period"},
    {GLFW_KEY_SLASH, "Key_Slash"},

    {GLFW_KEY_0, "Key_0"},
    {GLFW_KEY_1, "Key_1"},
    {GLFW_KEY_2, "Key_2"},
    {GLFW_KEY_3, "Key_3"},
    {GLFW_KEY_4, "Key_4"},
    {GLFW_KEY_5, "Key_5"},
    {GLFW_KEY_6, "Key_6"},
    {GLFW_KEY_7, "Key_7"},
    {GLFW_KEY_8, "Key_8"},
    {GLFW_KEY_9, "Key_9"},

    {GLFW_KEY_SEMICOLON, "Key_Semicolon"},
    {GLFW_KEY_EQUAL, "Key_Equal"},

    {GLFW_KEY_A, "Key_A"},
    {GLFW_KEY_B, "Key_B"},
    {GLFW_KEY_C, "Key_C"},
    {GLFW_KEY_D, "Key_D"},
    {GLFW_KEY_E, "Key_E"},
    {GLFW_KEY_F, "Key_F"},
    {GLFW_KEY_G, "Key_G"},
    {GLFW_KEY_H, "Key_H"},
    {GLFW_KEY_I, "Key_I"},
    {GLFW_KEY_J, "Key_J"},
    {GLFW_KEY_K, "Key_K"},
    {GLFW_KEY_L, "Key_L"},
    {GLFW_KEY_M, "Key_M"},
    {GLFW_KEY_N, "Key_N"},
    {GLFW_KEY_O, "Key_O"},
    {GLFW_KEY_P, "Key_P"},
    {GLFW_KEY_Q, "Key_Q"},
    {GLFW_KEY_R, "Key_R"},
    {GLFW_KEY_S, "Key_S"},
    {GLFW_KEY_T, "Key_T"},
    {GLFW_KEY_U, "Key_U"},
    {GLFW_KEY_V, "Key_V"},
    {GLFW_KEY_W, "Key_W"},
    {GLFW_KEY_X, "Key_X"},
    {GLFW_KEY_Y, "Key_Y"},
    {GLFW_KEY_Z, "Key_Z"},

    {GLFW_KEY_LEFT_BRACKET, "Key_LeftBracket"},
    {GLFW_KEY_BACKSLASH, "Key_Backslash"},
    {GLFW_KEY_RIGHT_BRACKET, "Key_RightBracket"},
    {GLFW_KEY_GRAVE_ACCENT, "Key_GraveAccent"},

    {GLFW_KEY_WORLD_1, "Key_World1"},
    {GLFW_KEY_WORLD_2, "Key_World2"},

    {GLFW_KEY_ESCAPE, "Key_Escape"},
    {GLFW_KEY_ENTER, "Key_Enter"},
    {GLFW_KEY_TAB, "Key_Tab"},
    {GLFW_KEY_BACKSPACE, "Key_Backspace"},
    {GLFW_KEY_INSERT, "Key_Insert"},
    {GLFW_KEY_DELETE, "Key_Delete"},

    {GLFW_KEY_RIGHT, "Key_Right"},
    {GLFW_KEY_LEFT, "Key_Left"},
    {GLFW_KEY_DOWN, "Key_Down"},
    {GLFW_KEY_UP, "Key_Up"},

    {GLFW_KEY_PAGE_UP, "Key_PageUp"},
    {GLFW_KEY_PAGE_DOWN, "Key_PageDown"},
    {GLFW_KEY_HOME, "Key_PageHome"},
    {GLFW_KEY_END, "Key_PageEnd"},

    {GLFW_KEY_CAPS_LOCK, "Key_CapsLock"},
    {GLFW_KEY_SCROLL_LOCK, "Key_ScrollLock"},
    {GLFW_KEY_NUM_LOCK, "Key_NumLock"},
    {GLFW_KEY_PRINT_SCREEN, "Key_PrintScreen"},
    {GLFW_KEY_PAUSE, "Key_Pause"},

    {GLFW_KEY_F1, "Key_F1"},
    {GLFW_KEY_F2, "Key_F2"},
    {GLFW_KEY_F3, "Key_F3"},
    {GLFW_KEY_F4, "Key_F4"},
    {GLFW_KEY_F5, "Key_F5"},
    {GLFW_KEY_F6, "Key_F6"},
    {GLFW_KEY_F7, "Key_F7"},
    {GLFW_KEY_F8, "Key_F8"},
    {GLFW_KEY_F9, "Key_F9"},
    {GLFW_KEY_F10, "Key_F10"},
    {GLFW_KEY_F11, "Key_F11"},
    {GLFW_KEY_F12, "Key_F12"},
    {GLFW_KEY_F13, "Key_F13"},
    {GLFW_KEY_F14, "Key_F14"},
    {GLFW_KEY_F15, "Key_F15"},
    {GLFW_KEY_F16, "Key_F16"},
    {GLFW_KEY_F17, "Key_F17"},
    {GLFW_KEY_F18, "Key_F18"},
    {GLFW_KEY_F19, "Key_F19"},
    {GLFW_KEY_F20, "Key_F20"},
    {GLFW_KEY_F21, "Key_F21"},
    {GLFW_KEY_F22, "Key_F22"},
    {GLFW_KEY_F23, "Key_F23"},
    {GLFW_KEY_F24, "Key_F24"},
    {GLFW_KEY_F25, "Key_F25"},

    {GLFW_KEY_KP_0, "Keypad_0"},
    {GLFW_KEY_KP_1, "Keypad_1"},
    {GLFW_KEY_KP_2, "Keypad_2"},
    {GLFW_KEY_KP_3, "Keypad_3"},
    {GLFW_KEY_KP_4, "Keypad_4"},
    {GLFW_KEY_KP_5, "Keypad_5"},
    {GLFW_KEY_KP_6, "Keypad_6"},
    {GLFW_KEY_KP_7, "Keypad_7"},
    {GLFW_KEY_KP_8, "Keypad_8"},
    {GLFW_KEY_KP_9, "Keypad_9"},

    {GLFW_KEY_KP_DECIMAL, "Keypad_Decimal"},
    {GLFW_KEY_KP_DIVIDE, "Keypad_Divide"},
    {GLFW_KEY_KP_MULTIPLY, "Keypad_Multiply"},
    {GLFW_KEY_KP_SUBTRACT, "Keypad_Subtract"},
    {GLFW_KEY_KP_ADD, "Keypad_Add"},
    {GLFW_KEY_KP_ENTER, "Keypad_Enter"},
    {GLFW_KEY_KP_EQUAL, "Keypad_Equal"},

    {GLFW_KEY_LEFT_SHIFT, "Key_LeftShift"},
    {GLFW_KEY_LEFT_CONTROL, "Key_LeftCtrl"},
    {GLFW_KEY_LEFT_ALT, "Key_LeftAlt"},
    {GLFW_KEY_LEFT_SUPER, "Key_LeftSuper"},

    {GLFW_KEY_RIGHT_SHIFT, "Key_RightShift"},
    {GLFW_KEY_RIGHT_CONTROL, "Key_RightCtrl"},
    {GLFW_KEY_RIGHT_ALT, "Key_RightAlt"},
    {GLFW_KEY_RIGHT_SUPER, "Key_RightSuper"},

    {GLFW_KEY_MENU, "Key_Menu"},

    {MouseEnumStart + GLFW_MOUSE_BUTTON_1, "MouseButton_Left"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_2, "MouseButton_Right"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_3, "MouseButton_Middle"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_4, "MouseButton_X1"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_5, "MouseButton_X2"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_6, "MouseButton_X3"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_7, "MouseButton_X4"},
    {MouseEnumStart + GLFW_MOUSE_BUTTON_8, "MouseButton_X5"},

    {InputHwCode(MouseInputId::MoveX), "MouseMove_X"},
    {InputHwCode(MouseInputId::MoveY), "MouseMove_Y"},

    {InputHwCode(MouseInputId::PosX), "MousePos_X"},
    {InputHwCode(MouseInputId::PosY), "MousePos_Y"},

    {InputHwCode(MouseInputId::WheelX), "MouseWheel_X"},
    {InputHwCode(MouseInputId::WheelY), "MouseWheel_Y"},

    {InputHwCode(MouseInputId::ENUM_END), "Undefined"},
  };
}

void
InputManager::load(
  const Path& configPath )
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
  const Path& configPath ) const
{
  auto inputConfig = serialize();

  LOG_TRACE("Writing input config to '{}'",
            configPath.string());

  const auto streamFlags =
    std::ios::out |
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
  using compos::SubscriberUpdate;
  using compos::CasqadiumEditorInternal;

  const auto inputHwId = mHwControlMap.find(inputHwCode);

  if ( inputHwId == mHwControlMap.end() )
    return;

  const std::string prefix =
    direction > 0
      ? "+"
      : direction < 0
          ? "-" : "~";

  const InputHwId inputId = prefix + inputHwId->second.str();

  const bool isMouseMotion =
    inputHwCode >= static_cast <InputHwCode> (MouseInputId::MoveX);

  if ( direction != 0 )
  {
    InputEvent event {};

    event.inputId = inputId;
    event.amount = amount;

    mInputHistory.push_back(event);

    if ( inputHwCode < static_cast <InputHwCode> (MouseInputId::MoveX) )
    {
      if ( direction > 0 )
        mInputsHeld[inputHwCode] = event.tp;
      else
        mInputsHeld.erase(inputHwCode);
    }
  }

  while ( mInputHistory.size() > mInputHistoryLength )
    mInputHistory.pop_front();

  auto& callbackMgr = registry.ctx().get <CallbackManager> ();

  const auto [axesBegin, axesEnd] = mBindings.equal_range(inputId);

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
  {
    auto& [binding, axisId] = *iter;

    const auto bindingInputHandler =
    [&, binding = binding,
        axisId = axisId] ( const auto entity, auto& cController )
    {
      if ( const auto& iter = cController.axes.find(axisId);
           iter != cController.axes.end() )
      {
        auto& [id, axis] = *iter;

        binding->handleInput(axis, amount);

        for ( const auto& callbackId : axis.callbacks )
          callbackMgr.execute(
            callbackId, registry,
            {entity, &cController, &axis, isMouseMotion} );
      }
    };

    if ( registry.storage <CasqadiumEditorInternal> ().empty() == true )
      registry.view <InputController, SubscriberUpdate, SubscriberInput> ().each(bindingInputHandler);
    else
      registry.view < InputController, SubscriberUpdate, SubscriberInput,
                      CasqadiumEditorInternal> ().each(bindingInputHandler);
  }
}

void
InputManager::updateInput(
  entt::registry& registry )
{
  for ( const auto& [inputCode, timestamp] : mInputsHeld )
    if ( timestamp < mLastPollTimestamp )
      handleAxisInput(inputCode, 1.0f, 0, registry);

  mLastPollTimestamp = TimeUtils::Now();
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
