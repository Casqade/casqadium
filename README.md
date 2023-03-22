# Casqadium Engine

ECS-based WIP game engine for personal usage. 

This project was born with the idea of a framework for developing small interactive applications and prototypes. 
Since its first days, gaining experience at software architecture and computer graphics continues to be the driving factor of this endeavour. 

Listed below are major initial design goals and requirements of the engine, some of which proved to be disputable over the course of development:
- Builds on Windows & Linux into a compact static binary
- Logic driven by entity component system architecture pattern
- Fully serialisable runtime state
- Human-readable game data & resource specification format (JSON in particular)
- Package system for game data & asset organisation
- Built-in GUI editing tools
- Cutting-edge graphics (textured quads, no time for fancy Blender stuff!)

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

The project is currently undergoing a transition from modified [olcPGE](https://github.com/OneLoneCoder/olcPixelGameEngine) to [OpenGL](https://github.com/Dav1dde/glad) + [GLFW](https://github.com/glfw/glfw) backends. 

Since there are far more advanced engines with fantastic graphics out there, it was decided to stick with minimalistic style and extend it with proper 3D model support (WIP also) for complementing textured quads primitives with low-poly meshes. 


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
