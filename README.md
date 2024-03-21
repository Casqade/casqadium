# Casqadium Engine

![release-build](https://github.com/casqade/casqadium/actions/workflows/release-build.yml/badge.svg)

ECS-based WIP game engine for personal usage. 

This project was born with the idea of a framework for developing small interactive applications and prototypes. 
Since its first days, gaining experience at software architecture and computer graphics continues to be the driving factor of this endeavour. 

Listed below are engine's major initial design goals and requirements, some of which proved to be disputable over the course of development:
- Builds on Windows & Linux into a compact static binary
- Logic driven by entity component system architecture pattern
- Fully serialisable runtime state
- Human-readable game data & resource specification format (JSON in particular)
- Package system for game data & asset organisation
- Built-in GUI editing tools
- Cutting-edge graphics (i.e. textured quads, no time for fancy Blender stuff!)

Since there are far more advanced engines featuring fantastic graphics out there, it was decided to stick with minimalistic style of textured quads and low-poly meshes. 

## Credits

Notable libraries used:
- [EnTT](https://github.com/skypjack/entt)
- [GLM](https://github.com/g-truc/glm)
- [JsonCPP](https://github.com/open-source-parsers/jsoncpp)
- [spdlog](https://github.com/gabime/spdlog)
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)
- [SoLoud](https://github.com/jarikomppa/soloud)

## Status

The project is currently being polished and tested in practice. 

A minimal running example can be found on the main branch, while most up-to-date demo application resides at 'demo-audio' branch. Another demo coming soon... 

Future plans: 
- Implement minimal lighting
- Migrate from [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d) to [Bullet3](https://github.com/bulletphysics/bullet3)

## Dependencies

### Windows MSYS2:

Default GCC toolchain should suffice

### Windows MSVC:

Haven't tested yet :(

### Arch Linux:
```
pacman -S libpng mesa
```

### Debian-based:
```
sudo apt install libglu1-mesa-dev libpng-dev
```
