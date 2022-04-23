#ifndef ECS_COMPONENTS_TEXTURE_STORAGE_HPP
#define ECS_COMPONENTS_TEXTURE_STORAGE_HPP

#include <ECS/Alias.hpp>
#include <ECS/Common.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>


namespace ECS
{

namespace Components
{

struct TextureStorage
{
  std::map <ECS::TextureId, std::shared_ptr <olc::Renderable>,
            entt::hashed_string_comparator> textures;
};

} // namespace Components

} // namespace ECS

#endif
