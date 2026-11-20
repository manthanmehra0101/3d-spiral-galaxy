#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

const int WIDTH = 1100;
const int HEIGHT = 760;
const int STAR_COUNT = 5200;
const float PI = 3.14159265f;

struct Star {
    float x, y, z;
    int red, green, blue;
    int size;
};

vector<Star> stars;

float randomFloat(float low, float high) {
    return low + (high - low) *
        (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

// Approximate a bell-shaped random value by adding several random values.
float randomSpread(float amount) {
    float value = 0.0f;
    for (int i = 0; i < 6; i++) {
        value += randomFloat(-1.0f, 1.0f);
    }
    return value * amount / 3.0f;
}

void addStar(float x, float y, float z, int red, int green, int blue, int size) {
    stars.push_back({x, y, z, red, green, blue, size});
}

void createGalaxy() {
    stars.clear();
    stars.reserve(STAR_COUNT);

    // Main spiral disk: stars follow one of four curved arms.
    const int armCount = 4;
    const int diskStars = 4100;

    for (int i = 0; i < diskStars; i++) {
        int arm = i % armCount;
        float distance = 18.0f + 270.0f * sqrt(randomFloat(0.0f, 1.0f));

        // The angle gradually changes with distance, producing curved arms.
        float armAngle = arm * (2.0f * PI / armCount);
        float angle = armAngle + distance * 0.017f;
        angle += randomSpread(0.13f);

        // Keep the stars close to the arm, with a small amount of natural scatter.
        float spread = 4.0f + distance * 0.045f;
        float x = cos(angle) * distance + randomSpread(spread);
        float y = sin(angle) * distance + randomSpread(spread);
        float z = randomSpread(5.0f + distance * 0.025f);

        float outer = distance / 288.0f;
        int red = static_cast<int>(150 + 70 * (1.0f - outer));
        int green = static_cast<int>(175 + 55 * (1.0f - outer));
        int blue = static_cast<int>(225 + 30 * (1.0f - outer));
        int size = (rand() % 100 < 12) ? 2 : 1;

        addStar(x, y, z, red, green, blue, size);
    }

    // Central bulge: a dense, warm-coloured collection of stars.
    for (int i = 0; i < 850; i++) {
        float angle = randomFloat(0.0f, 2.0f * PI);
        float radius = 48.0f * sqrt(randomFloat(0.0f, 1.0f));
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        float z = randomSpread(18.0f);

        int red = 255;
        int green = 185 + rand() % 65;
        int blue = 105 + rand() % 90;
        int size = (rand() % 100 < 38) ? 2 : 1;

        addStar(x, y, z, red, green, blue, size);
    }

    // A sparse halo gives the galaxy a little depth beyond its bright disk.
    for (int i = 0; i < 250; i++) {
        float angle = randomFloat(0.0f, 2.0f * PI);
        float radius = randomFloat(210.0f, 340.0f);
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        float z = randomSpread(45.0f);

        addStar(x, y, z, 125, 155, 205, 1);
    }
}

void rotateX(float &y, float &z, float angle) {
    float oldY = y;
    float oldZ = z;
    y = oldY * cos(angle) - oldZ * sin(angle);
    z = oldY * sin(angle) + oldZ * cos(angle);
}

void rotateY(float &x, float &z, float angle) {
    float oldX = x;
    float oldZ = z;
    x = oldX * cos(angle) + oldZ * sin(angle);
    z = -oldX * sin(angle) + oldZ * cos(angle);
}

void rotateZ(float &x, float &y, float angle) {
    float oldX = x;
    float oldY = y;
    x = oldX * cos(angle) - oldY * sin(angle);
    y = oldX * sin(angle) + oldY * cos(angle);
}

bool projectPoint(float x, float y, float z, int &screenX, int &screenY,
                  float scale, float moveX, float moveY, float moveZ) {
    x += moveX;
    y += moveY;
    z += moveZ;

    float depth = z + 650.0f;
    if (depth < 40.0f) {
        return false;
    }

    float perspective = 650.0f / depth;
    screenX = static_cast<int>(WIDTH / 2 + x * perspective * scale);
    screenY = static_cast<int>(HEIGHT / 2 + y * perspective * scale * 0.82f);

    return screenX >= 2 && screenX < WIDTH - 2 &&
           screenY >= 2 && screenY < HEIGHT - 2;
}

void drawGlow(SDL_Renderer *renderer, int centerX, int centerY, int radius,
              int red, int green, int blue) {
    // Several faint rings create a soft central glow using the CPU renderer.
    for (int r = radius; r > 0; r -= 3) {
        int alpha = static_cast<int>(5 + 20.0f * (1.0f - float(r) / radius));
        SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
        for (int x = -r; x <= r; x += 2) {
            int y = static_cast<int>(sqrt(max(0, r * r - x * x)));
            SDL_RenderDrawPoint(renderer, centerX + x, centerY + y);
            SDL_RenderDrawPoint(renderer, centerX + x, centerY - y);
        }
    }
}

void drawStar(SDL_Renderer *renderer, int x, int y, const Star &star, float depth) {
    float brightness = max(0.55f, min(1.25f, 650.0f / depth));
    int r = min(255, static_cast<int>(star.red * brightness));
    int g = min(255, static_cast<int>(star.green * brightness));
    int b = min(255, static_cast<int>(star.blue * brightness));

    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderDrawPoint(renderer, x, y);

    if (star.size > 1) {
        SDL_RenderDrawPoint(renderer, x - 1, y);
        SDL_RenderDrawPoint(renderer, x + 1, y);
        SDL_RenderDrawPoint(renderer, x, y - 1);
        SDL_RenderDrawPoint(renderer, x, y + 1);
    }
}

int main(int argc, char *argv[]) {
    srand(static_cast<unsigned int>(time(nullptr)));

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL could not start: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "3D Spiral Galaxy - Computer Graphics Project",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        cout << "Window could not be created: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
        0, WIDTH, HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888
    );

    if (surface == nullptr) {
        cout << "Surface could not be created: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    if (renderer == nullptr) {
        cout << "Software renderer could not be created: "
             << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    createGalaxy();

    float angleX = 0.72f;
    float angleY = 0.0f;
    float angleZ = 0.0f;
    float scale = 1.0f;
    float moveX = 0.0f, moveY = 0.0f, moveZ = 0.0f;
    bool running = true;
    bool autoRotate = true;

    cout << "\n3D SPIRAL GALAXY CONTROLS\n";
    cout << "Arrow keys : rotate X and Y\n";
    cout << "J / L      : rotate Z\n";
    cout << "+ / -      : zoom in / out\n";
    cout << "W A S D    : translate on screen\n";
    cout << "Q / E      : translate in depth\n";
    cout << "Space      : pause / resume rotation\n";
    cout << "R          : reset view\n";
    cout << "Esc        : quit\n\n";

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE: running = false; break;
                    case SDLK_UP: angleX -= 0.08f; break;
                    case SDLK_DOWN: angleX += 0.08f; break;
                    case SDLK_LEFT: angleY -= 0.08f; break;
                    case SDLK_RIGHT: angleY += 0.08f; break;
                    case SDLK_j: angleZ -= 0.08f; break;
                    case SDLK_l: angleZ += 0.08f; break;
                    case SDLK_PLUS:
                    case SDLK_EQUALS: scale += 0.1f; break;
                    case SDLK_MINUS:
                        if (scale > 0.2f) scale -= 0.1f;
                        break;
                    case SDLK_w: moveY -= 10.0f; break;
                    case SDLK_s: moveY += 10.0f; break;
                    case SDLK_a: moveX -= 10.0f; break;
                    case SDLK_d: moveX += 10.0f; break;
                    case SDLK_q: moveZ -= 10.0f; break;
                    case SDLK_e: moveZ += 10.0f; break;
                    case SDLK_SPACE: autoRotate = !autoRotate; break;
                    case SDLK_r:
                        angleX = 0.72f;
                        angleY = angleZ = 0.0f;
                        scale = 1.0f;
                        moveX = moveY = moveZ = 0.0f;
                        break;
                }
            }
        }

        if (autoRotate) {
            angleY += 0.0025f;
        }

        SDL_SetRenderDrawColor(renderer, 2, 3, 12, 255);
        SDL_RenderClear(renderer);

        // Draw the warm glow behind the central bulge.
        drawGlow(renderer, WIDTH / 2, HEIGHT / 2, 82, 255, 145, 65);

        for (const Star &star : stars) {
            float x = star.x;
            float y = star.y;
            float z = star.z;

            rotateX(y, z, angleX);
            rotateY(x, z, angleY);
            rotateZ(x, y, angleZ);

            int screenX, screenY;
            float depth = z + moveZ + 650.0f;
            if (projectPoint(x, y, z, screenX, screenY,
                             scale, moveX, moveY, moveZ)) {
                drawStar(renderer, screenX, screenY, star, depth);
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Surface *windowSurface = SDL_GetWindowSurface(window);
        if (windowSurface != nullptr) {
            SDL_BlitSurface(surface, nullptr, windowSurface, nullptr);
            SDL_UpdateWindowSurface(window);
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(surface);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
