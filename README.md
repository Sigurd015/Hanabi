# Hanabi

A simple game engine made for learning rendering APIs, game engine architecture, etc.

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)

![](https://github.com/Sigurd015/Hanabi/blob/main/images/preview.png)

## Goals
- Support multiple rendering APIs (such as DX12, Vulkan, Metal, etc.)
- Support full workflow, you can use this to make a game.

## Features
- [ ] Supported platforms
  - [x] Windows
  - [ ] Mac

- [ ] Rendering APIs
  - [ ] Vulkan
  - [x] DirectX 11
  - [ ] DirectX 12
  - [ ] Metal

- [x] Core
  - [x] Entry point
  - [x] Logging System
  - [x] Event system
  - [x] Scripting System
    - [x] C# Script Engine
  - [x] Serialize & Deserialize System (YAML)
  - [x] ECS System (entt)
  - [ ] Renderer 
    - [x] 2D Batch Renderer
    - [ ] 3D Renderer
      - [x] Deferred Rendering 
      - [ ] .....

- [ ] Editor  
  - [x] Play in editor
  - [ ] .....


## Dependencies
- [glfw](https://github.com/glfw/glfw)
- [glad](https://github.com/Dav1dde/glad)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [box2d](https://github.com/erincatto/box2d)
- [entt](https://github.com/skypjack/entt)
- [filewatch](https://github.com/ThomasMonkman/filewatch)
- [glm](https://github.com/g-truc/glm/tree/master)
- [ImGui](https://github.com/ocornut/imgui)
- [mono](https://github.com/mono/mono)
- [freetype](https://github.com/freetype/freetype)
- [msdfgen](https://github.com/Chlumsky/msdfgen)
- [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen)
- [spdlog](https://github.com/gabime/spdlog)
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)

## Roadmap
- [X] DX11 Support
- [X] C# Script Engine
- [ ] SPIR-V Shader System
- [ ] UI Renderer
- [X] Asset Manager
- [ ] 3D Renderer
  - [X] Mesh System
  - [X] Material System
  - [X] Lighting
  - [ ] Shadow mapping
    - [ ] Cascaded Shadow Maps
    - [ ] Omnidirectional Shadow Maps
  - [x] Skyboxes
  - [ ] Physically based rendering (PBR)
  - [ ] Screen space ambient occulsion
  - [ ] Screen space reflection
- [ ] Ray/Path tracing
  - [ ] Vulkan & DX12
- [ ] Metal support
- [ ] Runtime
  - [ ] Asset Package Serialize & Deserialize

## Getting Started

Visual Studio 2022 is recommended

<ins>**1. Downloading the repository:**</ins>

```
git clone https://github.com/Sigurd015/Hanabi
git submodule update --init --recursive
```

<ins>**2. Run the [Win-GenProjects.bat](https://github.com/Sigurd015/Hanabi/blob/main/Win-GenProjects.bat)**</ins>

You can change the .bat file to generate a different version project file, but that is not tested.

<ins>**3. Open .sln file with Visual Studio**</ins>

## Credits

- [TheCherno](https://www.youtube.com/@TheCherno) [Hazel](https://github.com/TheCherno/Hazel)
