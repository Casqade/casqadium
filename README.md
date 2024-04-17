# Casqadium Engine

[![Windows (MSVC)](https://github.com/casqade/casqadium/actions/workflows/windows-build-msvc.yml/badge.svg)](https://github.com/casqade/casqadium/actions/workflows/windows-build-msvc.yml)
[![Windows (MSYS2)](https://github.com/casqade/casqadium/actions/workflows/windows-build-msys2.yml/badge.svg)](https://github.com/casqade/casqadium/actions/workflows/windows-build-msys2.yml)
[![Ubuntu](https://github.com/casqade/casqadium/actions/workflows/ubuntu-build.yml/badge.svg)](https://github.com/casqade/casqadium/actions/workflows/ubuntu-build.yml)
[![macOS](https://github.com/casqade/casqadium/actions/workflows/macos-build.yml/badge.svg)](https://github.com/casqade/casqadium/actions/workflows/macos-build.yml)
[![GitHub Releases](https://img.shields.io/github/release/casqade/casqadium.svg)](https://github.com/casqade/casqadium/releases/latest)

This is an ECS-based game engine for personal usage.
It was born as a framework for developing
small interactive applications and prototypes.

Listed below are engine's major
initial design goals and requirements,
some of which proved to be disputable
over the course of development:
- Builds on Windows & Linux into a compact static binary
- Logic driven by entity component system architecture pattern
- Fully serialisable runtime state
- Human-readable entity & resource specification format (JSON)
- Package system for bundling scenes & assets
- Built-in GUI editing tools
- Minimalistic style graphics (i.e. textured quads & low poly meshes)


## Downloads

[Windows binaries](https://github.com/casqade/casqadium/releases)

[Demo examples](https://github.com/casqade/casqadium-demos)


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


## Building:

### Windows (MSYS2 mingw32 environment):

```bash
sudo pacman -S git
sudo pacman -S mingw-w64-i686-cmake mingw-w64-i686-gcc
git clone https://github.com/casqade/casqadium
cd casqadium
cmake .
cmake --build .
```

### Windows (MSVC):

Builds with default Visual Studio Tools installation,
but you must have CMake installed

```bash
git clone https://github.com/casqade/casqadium
cd casqadium
cmake .
cmake --build .
```

### Arch Linux:

```bash
pacman -S git gcc cmake libpng mesa
git clone https://github.com/casqade/casqadium
cd casqadium
cmake .
cmake --build .
```

### Ubuntu/Debian-based:

```bash
sudo apt install git g++ cmake ninja-build xorg-dev libasound2-dev libglu1-mesa-dev libpng-dev
git clone https://github.com/casqade/casqadium
cd casqadium
cmake .
cmake --build .
```

### macOS (Sierra 10.12 and newer):

[macOS workflow](https://github.com/casqade/casqadium/actions/workflows/macos-build.yml)
builds fine, but I don't have any
MacBooks for runtime testing,
so feel free to report any issues (or successes)

```bash
brew install git cmake
git clone https://github.com/casqade/casqadium
cd casqadium
cmake .
cmake --build .
```

