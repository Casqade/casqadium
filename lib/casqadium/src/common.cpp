#include <cqde/common.hpp>
#include <cqde/components/InputController.hpp>
#include <cqde/components/SceneNode.hpp>
#include <cqde/components/Tag.hpp>
#include <cqde/components/Transform.hpp>
#include <cqde/types/EntityTagStorage.hpp>
#include <cqde/types/InputCallbackStorage.hpp>

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
    {olc::Key::NONE, cqde::InputSourceId("Key_None")},
    {olc::Key::A, cqde::InputSourceId("Key_A")},
    {olc::Key::D, cqde::InputSourceId("Key_D")},
    {olc::Key::S, cqde::InputSourceId("Key_S")},
    {olc::Key::W, cqde::InputSourceId("Key_W")},
    {olc::Key::SHIFT, cqde::InputSourceId("Key_Shift")},
    {olc::Key::SPACE, cqde::InputSourceId("Key_Space")},
    {olc::Key::ENTER, cqde::InputSourceId("Key_Enter")},

    {int32_t(MouseInputId::ButtonLeft), cqde::InputSourceId("MouseButton_Left")},
    {int32_t(MouseInputId::ButtonRight), cqde::InputSourceId("MouseButton_Right")},
    {int32_t(MouseInputId::ButtonMiddle), cqde::InputSourceId("MouseButton_Middle")},
    {int32_t(MouseInputId::ButtonX1), cqde::InputSourceId("MouseButton_X1")},
    {int32_t(MouseInputId::ButtonX2), cqde::InputSourceId("MouseButton_X2")},
    {int32_t(MouseInputId::MoveX), cqde::InputSourceId("Mouse_MoveX")},
    {int32_t(MouseInputId::MoveY), cqde::InputSourceId("Mouse_MoveY")},
    {int32_t(MouseInputId::Wheel), cqde::InputSourceId("Wheel_Move")},
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

  registry.set <EntityTagStorage> ();
  registry.set <InputCallbackStorage> ();
  registry.set <TextureStorage> ().emplace( null_id,
                                            std::make_shared <olc::Renderable> () );

  initHwControls( registry.set <HwControlMap> () );
  registry.set <SwControlMap> ();
}


} // namespace cqde
