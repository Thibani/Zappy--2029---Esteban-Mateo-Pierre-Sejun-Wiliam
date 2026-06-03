#include <raylib.h>
#include "Character/Character.hpp"
#include "Map/Map.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Constants.hpp"

int main() {
    InitWindow(1280, 720, "Zappy GUI");
    SetTargetFPS(60);
    Character::CharacterFactory factory;
    factory.createCharacter("Player1", 5, 5);
    factory.createCharacter("Player2", 10, 15);

    // Hardcoded for now — will come from msz once server is connected
    Map    map(20, 20);
    PlayerView camera;
    camera.init(map.getWidth()  * TILE_SIZE,
                map.getHeight() * TILE_SIZE);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        camera.handleInput(dt);

        BeginDrawing();
            ClearBackground({ 20, 20, 30, 255 });

            BeginMode3D(camera.get());
                Renderer::drawMap(map, factory, camera);
            EndMode3D();

            // HUD (2D overlay, always on top)
            DrawFPS(10, 10);
            DrawText("WASD: pan | RMB drag / Q-E: orbit | R-F: tilt | Scroll: zoom",
                     10, 40, 16, RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}