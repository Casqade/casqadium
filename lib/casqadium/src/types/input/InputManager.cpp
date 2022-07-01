#include <cqde/types/input/InputManager.hpp>

#include <cqde/common.hpp>
#include <cqde/util/logger.hpp>

#include <cqde/components/InputController.hpp>

#include <cqde/types/input/InputCallbackStorage.hpp>
#include <cqde/types/input/InputBindingAbsolute.hpp>
#include <cqde/types/input/InputBindingRelative.hpp>


#include <olcPGE/olcMouseInputId.hpp>

#include <entt/entity/registry.hpp>

#include <json/value.h>


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
  using olc::MouseInputId;

  mHwControlMap =
  {
    {olc::Key::NONE, cqde::InputHwId("Key_Unknown")},
    {olc::Key::A, cqde::InputHwId("Key_A")},
    {olc::Key::D, cqde::InputHwId("Key_D")},
    {olc::Key::S, cqde::InputHwId("Key_S")},
    {olc::Key::W, cqde::InputHwId("Key_W")},
    {olc::Key::SHIFT, cqde::InputHwId("Key_Shift")},
    {olc::Key::SPACE, cqde::InputHwId("Key_Space")},
    {olc::Key::ENTER, cqde::InputHwId("Key_Enter")},

    {InputHwCode(MouseInputId::ButtonLeft), cqde::InputHwId("MouseButton_Left")},
    {InputHwCode(MouseInputId::ButtonRight), cqde::InputHwId("MouseButton_Right")},
    {InputHwCode(MouseInputId::ButtonMiddle), cqde::InputHwId("MouseButton_Middle")},
    {InputHwCode(MouseInputId::ButtonX1), cqde::InputHwId("MouseButton_X1")},
    {InputHwCode(MouseInputId::ButtonX2), cqde::InputHwId("MouseButton_X2")},

    {InputHwCode(MouseInputId::Wheel), cqde::InputHwId("MouseWheel_Y")},

    {InputHwCode(MouseInputId::MoveX), cqde::InputHwId("MouseMove_X")},
    {InputHwCode(MouseInputId::MoveY), cqde::InputHwId("MouseMove_Y")},
    {InputHwCode(MouseInputId::PosX), cqde::InputHwId("MousePos_X")},
    {InputHwCode(MouseInputId::PosY), cqde::InputHwId("MousePos_Y")},

    {InputHwCode(MouseInputId::ENUM_END) + 1, cqde::InputHwId("Undefined")},
  };
}

void
InputManager::parseInputConfigFile( const std::filesystem::path& configPath )
{
  Json::Value inputConfig {};

  LOG_TRACE("Parsing input config '{}'", configPath.string());

  try
  {
    inputConfig = fileParse(configPath);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to parse input config ({})",
                                    e.what()));
  }

  LOG_TRACE("Validating input config '{}'", configPath.string());

  try
  {
    jsonValidateObject(inputConfig, inputConfigReference);
  }
  catch ( const std::exception& e )
  {
    throw std::runtime_error(format("Failed to validate input config '{}': {}",
                                    configPath.string(), e.what()));
  }

  LOG_TRACE("Importing input config '{}'", configPath.string());

  for ( const auto& axisId : inputConfig.getMemberNames() )
  {
    LOG_INFO("Binding inputs to axis '{}' ('{}')", axisId, configPath.string());

    for ( const auto& bindingHwId : inputConfig[axisId].getMemberNames() )
    {
      try
      {
        assignBinding(axisId, InputBinding::FromJson(bindingHwId, inputConfig[axisId][bindingHwId]));
      }
      catch ( const std::exception& e )
      {
        throw std::runtime_error(format("Failed to validate input binding for axis '{}' in '{}': {}",
                                        axisId, configPath.string(), e.what()));
      }
    }
  }
}

void InputManager::assignBinding(
  const InputAxisId axisId,
  const std::shared_ptr <InputBinding> binding )
{
  mBindings.insert({ binding, axisId });
}

void InputManager::assignBindings(
  const InputAxisId axisId,
  const std::set <std::shared_ptr <InputBinding>,
                  InputBindingComparator>& bindings )
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
  const std::string inputDir  = direction > 0.0f
                              ? "+"
                              : "-";

  auto& inputCallbacks = registry.ctx().at <InputCallbackStorage> ();

  InputHwId inputId = mHwControlMap[inputHwCode];

  const auto [axesBegin, axesEnd] = mBindings.equal_range( inputDir + inputId.str() );

  for ( auto iter = axesBegin;
        iter != axesEnd;
        ++iter )
  {
    auto& [binding, axisId] = *iter;

    for ( auto&& [entity, cController] : registry.view <compos::InputController> ().each() )
      if ( const auto& iter = cController.inputs.find(axisId);
           iter != cController.inputs.end() )
      {
        auto& [id, axis] = *iter;

        binding->handleInput(axis, amount);

        for ( const auto& callbackId : axis.callbacks )
          inputCallbacks.Execute(callbackId, entity, cController);
      }
  }
}

} // namespace cqde::types
