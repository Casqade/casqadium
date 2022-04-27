#pragma once

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>


namespace ECS
{

namespace Types
{

struct TextureStorage
{
  std::map <ECS::TextureId, std::shared_ptr <olc::Renderable>,
            entt::hashed_string_comparator> textures;

  TextureStorage();
};

} // namespace Types

} // namespace ECS
