#include <cqde/common.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>

#include <cqde/types/input/InputBinding.hpp>
#include <cqde/types/EntityTagStorage.hpp>
#include <cqde/types/InputCallbackStorage.hpp>

#include <cqde/types/assets/FontAssetManager.hpp>
#include <cqde/types/assets/TextureAssetManager.hpp>

#include <entt/entity/registry.hpp>

#include <olcPGE/olcMouseInputId.hpp>


namespace cqde
{

void
initHwControls( types::HwControlMap& controlMap )
{
  using olc::MouseInputId;

  controlMap =
  {
    {olc::Key::NONE, cqde::InputHwId("Key_Unknown")},
    {olc::Key::A, cqde::InputHwId("Key_A")},
    {olc::Key::D, cqde::InputHwId("Key_D")},
    {olc::Key::S, cqde::InputHwId("Key_S")},
    {olc::Key::W, cqde::InputHwId("Key_W")},
    {olc::Key::SHIFT, cqde::InputHwId("Key_Shift")},
    {olc::Key::SPACE, cqde::InputHwId("Key_Space")},
    {olc::Key::ENTER, cqde::InputHwId("Key_Enter")},

    {int32_t(MouseInputId::ButtonLeft), cqde::InputHwId("MouseButton_Left")},
    {int32_t(MouseInputId::ButtonRight), cqde::InputHwId("MouseButton_Right")},
    {int32_t(MouseInputId::ButtonMiddle), cqde::InputHwId("MouseButton_Middle")},
    {int32_t(MouseInputId::ButtonX1), cqde::InputHwId("MouseButton_X1")},
    {int32_t(MouseInputId::ButtonX2), cqde::InputHwId("MouseButton_X2")},

    {int32_t(MouseInputId::Wheel), cqde::InputHwId("MouseWheel_Y")},

    {int32_t(MouseInputId::MoveX), cqde::InputHwId("MouseMove_X")},
    {int32_t(MouseInputId::MoveY), cqde::InputHwId("MouseMove_Y")},
    {int32_t(MouseInputId::PosX), cqde::InputHwId("MousePos_X")},
    {int32_t(MouseInputId::PosY), cqde::InputHwId("MousePos_Y")},

    {int32_t(MouseInputId::ENUM_END) + 1, cqde::InputHwId("Undefined")},
  };
}

void
engineInit( entt::registry& registry )
{
  using namespace cqde::compos;
  using namespace cqde::types;

  InputController::Register();
  SceneNode::Register();
  Tag::Register();
  Transform::Register();

  registry.ctx().emplace <EntityTagStorage> ();
  registry.ctx().emplace <InputCallbackStorage> ();

  initHwControls( registry.ctx().emplace <HwControlMap> () );
  registry.ctx().emplace <InputBindings> ();

  registry.ctx().emplace <FontAssetManager> ();
  registry.ctx().emplace <TextureAssetManager> ();
}


} // namespace cqde
