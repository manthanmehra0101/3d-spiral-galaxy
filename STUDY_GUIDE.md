# Study Guide — 3D Spiral Galaxy

## 1. How the galaxy is generated

The program creates three populations of stars:

- **Spiral disk:** most stars are assigned to one of four arms. The arm angle changes with radius, so the arms curve instead of forming straight spokes.
- **Central bulge:** a dense group of warm-coloured stars is placed near the centre.
- **Halo:** a smaller number of faint stars are placed farther from the disk.

The small random offsets prevent the arms from looking like perfectly mathematical lines.

## 2. Transformations

### Rotation around Z

For an angle θ:

- x' = x cos(θ) − y sin(θ)
- y' = x sin(θ) + y cos(θ)

The same principle is implemented for rotation around X and Y.

### Translation

- x' = x + Tx
- y' = y + Ty
- z' = z + Tz

### Scaling

The scale value multiplies the projected X and Y coordinates, making the galaxy appear larger or smaller.

## 3. Perspective projection

The program uses a simple perspective factor:

`perspective = 650 / (z + 650)`

The screen position is calculated from the transformed 3D coordinates and this perspective factor. This makes points at different depths appear at different sizes and positions.

## 4. CPU rendering

`SDL_CreateSoftwareRenderer(surface)` creates a renderer that draws into an SDL surface using the CPU. The completed surface is copied to the window with `SDL_BlitSurface` and `SDL_UpdateWindowSurface`.

## 5. Viva questions to prepare

**Q: Why are there four spiral arms?**  
A: Each star is assigned an arm index from zero to three. The arm index supplies its starting angle, while the radius-dependent angle bends the arm.

**Q: What makes the model 3D?**  
A: Every star has X, Y and Z coordinates. The program rotates these coordinates in 3D and then applies perspective projection to display them on a 2D screen.

**Q: Why is projection necessary?**  
A: A monitor is a 2D surface, so 3D coordinates must be converted into 2D screen coordinates.

**Q: Is this GPU rendering?**  
A: No. SDL's software renderer draws to an in-memory surface using CPU processing.

**Q: Is this an exact astronomical simulation?**  
A: No. It is a stylized procedural model designed to demonstrate computer graphics concepts.
