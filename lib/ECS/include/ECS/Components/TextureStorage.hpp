#ifndef ECS_COMPONENTS_TEXTURE_STORAGE_HPP
#define ECS_COMPONENTS_TEXTURE_STORAGE_HPP

#include <entt/fwd.hpp>

#include <olcPGE/olcPixelGameEngine.hpp>

#include <map>
#include <string>


namespace ECS
{

namespace Components
{

struct TextureStorage
{
  std::map <ENTT_ID_TYPE, olc::Renderable> textures;
};

} // namespace Components

} // namespace ECS

#endif
