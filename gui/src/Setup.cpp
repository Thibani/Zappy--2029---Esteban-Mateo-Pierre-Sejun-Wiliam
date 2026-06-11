#include <raylib.h>
#include "Core/Window.hpp"
#include "Character/CharacterFactory.hpp"
#include "Map/Map.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Constants.hpp"

static void gameLoop(CharacterFactory& factory, const Map& map, PlayerView& camera, Renderer& renderer) {
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        camera.handleInput(dt);
        BeginDrawing();
        ClearBackground({ 20, 20, 30, 255 });
        BeginMode3D(camera.get());
        renderer.drawMap(map, factory, camera);
        EndMode3D();
        DrawFPS(10, 10);
        DrawText("WASD: pan | RMB drag / Q-E: orbit | R-F: tilt | Scroll: zoom",
            10, 40, 16, RAYWHITE);
        EndDrawing();
    }
}

void gameSetup() {
    Window window(1280, 720, "Zappy GUI");
    CharacterFactory factory;
    factory.createCharacter("Player1", 5, 5, 1);
    factory.createCharacter("Player2", 10, 15, 3);
    Map map(20, 20);
    PlayerView camera;
    camera.init(map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE);
    Renderer renderer("assets/Characters/Commander_lv7.png");
    gameLoop(factory, map, camera, renderer);
}