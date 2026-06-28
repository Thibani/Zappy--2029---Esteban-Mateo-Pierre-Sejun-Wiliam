#include "Renderer.hpp"
#include "../Constants.hpp"

Renderer::Renderer(AssetManager& assets) : _assets(assets) {}

void Renderer::drawMap(const Map& map, const CharacterFactory& factory, const EggFactory& eggs, PlayerView& camera)
{
    _drawTiles(map, camera);
    _drawEggs(eggs, camera);
    _drawCharacters(factory, camera);
}

void Renderer::_drawTiles(const Map& map, PlayerView& camera)
{
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            const Tile& tile = map.getTile(x, y);
            Vector3 pos = {
                x * TILE_SIZE + TILE_SIZE / 2.f,
                0.f,
                y * TILE_SIZE + TILE_SIZE / 2.f
            };
            Color color = DARKGREEN;
            DrawCube(pos, TILE_SIZE, 0.1f, TILE_SIZE, color);
            DrawCubeWires(pos, TILE_SIZE, 0.1f, TILE_SIZE, BLACK);

            Vector3 resPos = { pos.x, 0.15f, pos.z };
            if (tile.q0 > 0) DrawBillboard(camera.get(), _assets.get("food"),       resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q1 > 0) DrawBillboard(camera.get(), _assets.get("GoldOre"),    resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q2 > 0) DrawBillboard(camera.get(), _assets.get("IronOre"),    resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q3 > 0) DrawBillboard(camera.get(), _assets.get("Stone"),      resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q4 > 0) DrawBillboard(camera.get(), _assets.get("Fang"),       resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q5 > 0) DrawBillboard(camera.get(), _assets.get("Ruby"),       resPos, TILE_SIZE * 0.4f, WHITE);
            if (tile.q6 > 0) DrawBillboard(camera.get(), _assets.get("Starshard"),  resPos, TILE_SIZE * 0.4f, WHITE);
        }
    }
}

void Renderer::_drawCharacters(const CharacterFactory& factory, PlayerView& camera)
{
    for (const auto& c : factory.getAll()) {
        Vector3 pos = {
            c.tileX * TILE_SIZE + TILE_SIZE / 2.f,
            0.55f,
            c.tileY * TILE_SIZE + TILE_SIZE / 2.f
        };
        float size = TILE_SIZE * 0.9f * (1.0f + (c.level - 1) * 0.1f);
        std::string key = "character_l" + std::to_string(c.level);
        DrawBillboard(camera.get(), _assets.get(key), pos, size, WHITE);
    }
}

void Renderer::_drawEggs(const EggFactory& eggs, PlayerView& camera)
{
    for (const auto& e : eggs.getAll()) {
        Vector3 pos = {
            e.tileX * TILE_SIZE + TILE_SIZE / 2.f,
            0.15f,
            e.tileY * TILE_SIZE + TILE_SIZE / 2.f
        };
        DrawBillboard(camera.get(), _assets.get("HeiseneggBaby"), pos, TILE_SIZE * 0.6f, WHITE);
    }
}