#include <raylib.h>
#include <iostream>
#include "Core/Window.hpp"
#include "Character/CharacterFactory.hpp"
#include "Map/Map.hpp"
#include "Camera/Camera.hpp"
#include "Renderer/Renderer.hpp"
#include "Constants.hpp"
#include "Protocol/ServerParser.hpp"
#include "Network/NetworkClient.hpp"

static void gameLoop(CharacterFactory& factory, Map& map, EggFactory& eggs, PlayerView& camera, Renderer& renderer, NetworkClient& network) {
    ServerParser parser;

    while (!WindowShouldClose()) {
        std::string line;
        while (network.pollLine(line))
            parser.parse(line, map, factory, eggs);

        float dt = GetFrameTime();
        camera.handleInput(dt);
        BeginDrawing();
        ClearBackground({ 20, 20, 30, 255 });
        BeginMode3D(camera.get());
        renderer.drawMap(map, factory, eggs, camera);
        EndMode3D();
        DrawFPS(10, 10);
        DrawText("WASD: pan | RMB drag / Q-E: orbit | R-F: tilt | Scroll: zoom",
            10, 40, 16, RAYWHITE);
        EndDrawing();
    }
}

void gameSetup() {
    Window window(1280, 720, "Zappy GUI");
    Map map(20, 20);
    CharacterFactory factory;
    EggFactory eggs;
    PlayerView camera;
    camera.init(map.getWidth() * TILE_SIZE, map.getHeight() * TILE_SIZE);
    Renderer renderer("assets/Characters/Commander_lv7.png");

    NetworkClient network;
    if (!network.connect("127.0.0.1", 4242)) {
        std::cerr << "Failed to connect to server\n";
        return;
    }

    std::string line;
    while (!network.pollLine(line));
    network.send("GRAPHIC\n");

    gameLoop(factory, map, eggs, camera, renderer, network);
}