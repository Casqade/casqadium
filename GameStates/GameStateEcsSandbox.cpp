#include <GameStateEcsSandbox.hpp>
#include <GameStateController.hpp>

#include <TimeUtils/Duration.hpp>

#include <entt/entt.hpp>
#include <entt/core/hashed_string.hpp>

#include <ECS/Common.hpp>
#include <ECS/Components/Camera.hpp>
#include <ECS/Components/SceneNode.hpp>
#include <ECS/Components/Transform.hpp>
#include <ECS/Components/GeometryBuffer.hpp>
#include <ECS/Components/TextureBuffer.hpp>
#include <ECS/Components/TextureStorage.hpp>

#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Systems/CullingSystem.hpp>


GameStateEcsSandbox::GameStateEcsSandbox( GameStateController* const stateController )
  : GameState(stateController)
  , mState(StateLocal::Idle)
{
  using namespace entt::literals;
  using namespace ECS::Components;

  ECS::registryInit(mRegistry);

  ECS::AssetStorage = mRegistry.create();
  auto& textureStorage = mRegistry.emplace <TextureStorage> (ECS::AssetStorage);

  olc::Renderable texture = olc::Renderable();

  const olc::vi2d textSize = mPGE->GetTextSize( "T" );
  texture.Create( textSize.x, textSize.y );

  mPGE->SetDrawTarget( texture.Sprite() );
  mPGE->Clear( olc::BLANK );
  mPGE->DrawString( 0, 0, "T", olc::WHITE );
  mPGE->SetDrawTarget(nullptr);

  texture.Decal()->Update();

  textureStorage.textures.emplace("test"_hs, std::move(texture));

  auto eQuad = mRegistry.create();
  auto eCamera = mRegistry.create();

  mRegistry.emplace <Transform> (eQuad) = Transform({0.0f, 0.0f, -10.0f});
  mRegistry.emplace <SceneNode> (eQuad) = SceneNode();
  mRegistry.emplace <GeometryBuffer> (eQuad) = GeometryBuffer(defaultQuadVerts);
  auto& textureBuffer = mRegistry.emplace <TextureBuffer> (eQuad) = TextureBuffer();

  auto& cCamera = mRegistry.emplace <Camera> (eCamera) = Camera();
  mRegistry.emplace <Transform> (eCamera) = Transform();
  mRegistry.emplace <SceneNode> (eCamera) = SceneNode();

  cCamera.viewport = { 0.0f, 0.0f, mPGE->GetWindowSize().x, mPGE->GetWindowSize().y };

  textureBuffer.textures[0] = "test"_hs;
//  textureBuffer.textures[1] = "test"_hs;
}

bool
GameStateEcsSandbox::update(  const uint32_t ticks,
                              const TimeUtils::Duration elapsed )
{
  using namespace ECS::Components;

  static uint32_t ticks_total = 0;

  for ( auto&& [eCamera, cCamera, cTransform] : mRegistry.view <Camera, Transform>().each() )
  {
    if ( mPGE->GetKey(olc::Key::W).bHeld )
      cTransform.translation.z -= ticks * 0.1f;
  }

  return (ticks_total += ticks) < 6000;
}

void
GameStateEcsSandbox::render()
{
  ECS::Systems::CullSystem(mRegistry);
  ECS::Systems::RenderSystem(mRegistry);
}
