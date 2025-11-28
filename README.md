# Hanabi

Hanabi is a simple game engine built for learning game engine architecture, rendering systems, and more.

The project started with [Game Engine Series by The Cherno](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) and extends it with additional 3D features. If you are interested in game engine development, this video series is an excellent step-by-step guide.

If you want to explore this repository, follow the build instructions below.

## Build

### Windows

[Vulkan SDK](https://vulkan.lunarg.com/sdk/home) 1.2 or higher

Visual Studio 2022 or newer

<ins>**1. Clone the repository and initialize submodules:**</ins>

```
git clone https://github.com/Sigurd015/Hanabi
git submodule update --init --recursive
```

<ins>**2. Run the [Pre_Build.bat](Setup-Windows.bat) script**</ins>

<ins>**3. Generate Visual Studio solution with CMake (or build directly using CMake)**</ins>

```
cmake -G "Visual Studio 17 2022" -S . -B build
```
