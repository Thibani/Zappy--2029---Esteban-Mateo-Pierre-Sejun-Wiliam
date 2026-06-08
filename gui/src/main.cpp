#include <raylib.h>
#include "Character/Character.hpp"
#include "Map/Map.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Constants.hpp"

int main() {
    InitWindow(1280, 720, "Zappy GUI");
    TraceLog(LOG_INFO, "Working dir: %s", GetWorkingDirectory());
    SetTargetFPS(60);

    Character::CharacterFactory factory;
    factory.createCharacter("Player1", 5, 5);
    factory.createCharacter("Player2", 10, 15);

    Map map(20, 20);
    PlayerView camera;
    camera.init(map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE);

    // Load once here, NOT inside drawMap
    Renderer renderer;
    Texture2D charTexture = LoadTexture("assets/Characters/Commander_lv7.png");

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        camera.handleInput(dt);
        BeginDrawing();
        ClearBackground({ 20, 20, 30, 255 });
        BeginMode3D(camera.get());
        renderer.drawMap(map, factory, camera, charTexture); // pass it in
        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(charTexture); // free it after the loop
    CloseWindow();
    return 0;
}