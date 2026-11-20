# 3D Spiral Galaxy — Computer Graphics Project

A student-oriented C++17 project using SDL2's **software renderer**. The galaxy is generated procedurally and displayed using CPU rendering; it does not require an OpenGL context or a dedicated GPU.

## Build on CachyOS / Arch Linux

```bash
sudo pacman -S --needed base-devel cmake sdl2
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
./build/galaxy
```

## Controls

- Arrow keys: rotate around X and Y
- J / L: rotate around Z
- + / -: scale
- W / A / S / D: translate on screen
- Q / E: translate in depth
- Space: pause or resume automatic rotation
- R: reset
- Esc: quit

## Computer Graphics concepts

1. Procedural generation of four spiral arms, a central bulge and a sparse halo.
2. 3D rotation around X, Y and Z axes.
3. Translation along X, Y and Z.
4. Scaling.
5. Perspective projection from 3D coordinates to 2D screen coordinates.
6. CPU-based rasterization through SDL2's software renderer.

The galaxy is a stylized educational model, not an astrophysically accurate simulation.
