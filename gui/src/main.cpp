#include <raylib.h>
#include "Character/Character.hpp"
#include "Map/Map.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Constants.hpp"

// RAII wrapper for Raylib window
struct Window {
    Window(int w, int h, const char* title) { InitWindow(w, h, title); SetTargetFPS(60); }
    ~Window() { CloseWindow(); }
};

// RAII wrapper for Raylib texture
struct ManagedTexture {
    Texture2D texture;
    ManagedTexture(const char* path) { texture = LoadTexture(path); }
    ~ManagedTexture() { UnloadTexture(texture); }
    ManagedTexture(const ManagedTexture&) = delete;
    ManagedTexture& operator=(const ManagedTexture&) = delete;
};

void gameLoop(Character::CharacterFactory& factory, Map& map, PlayerView& camera, Renderer& renderer, Texture2D charTexture)
{
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        camera.handleInput(dt);
        BeginDrawing();
        ClearBackground({ 20, 20, 30, 255 });
        BeginMode3D(camera.get());
        renderer.drawMap(map, factory, camera, charTexture);
        EndMode3D();
        DrawFPS(10, 10);
        DrawText("WASD: pan | RMB drag / Q-E: orbit | R-F: tilt | Scroll: zoom",
            10, 40, 16, RAYWHITE);
        EndDrawing();
    }
}

void gameSetup()
{
    Character::CharacterFactory factory;
    factory.createCharacter("Player1", 5, 5);
    factory.createCharacter("Player2", 10, 15);

    Map map(20, 20);
    PlayerView camera;
    camera.init(map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE);

    Renderer renderer;
    ManagedTexture charTexture("assets/Characters/Commander_lv7.png");

    gameLoop(factory, map, camera, renderer, charTexture.texture);
    // ManagedTexture destructor fires here automatically
}

int main() {
    Window window(1280, 720, "Zappy GUI");
    gameSetup();
    // Window destructor fires here automatically
    return 0;
}