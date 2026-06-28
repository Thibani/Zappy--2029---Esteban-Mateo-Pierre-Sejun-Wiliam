#include "Renderer.hpp"
#include "../Constants.hpp"

Renderer::Renderer(AssetManager& assets) : _assets(assets) {}

void Renderer::drawMap(const Map& map, const CharacterFactory& factory, const EggFactory& eggs, PlayerView& camera)
{
    _drawTiles(map);
    _drawEggs(eggs);
    _drawCharacters(factory, camera);
}

void Renderer::_drawTiles(const Map& map)
{
    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            const Tile& tile = map.getTile(x, y);
            Vector3 pos = {
                x * TILE_SIZE + TILE_SIZE / 2.f,
                0.f,
                y * TILE_SIZE + TILE_SIZE / 2.f
            };
            Color color = (tile.q0 > 0) ? GREEN :
                          (tile.q1 > 0) ? YELLOW :
                          DARKGREEN;
            DrawCube(pos, TILE_SIZE, 0.1f, TILE_SIZE, color);
            DrawCubeWires(pos, TILE_SIZE, 0.1f, TILE_SIZE, BLACK);
        }
    }
}

void Renderer::_drawCharacters(const CharacterFactory& factory, PlayerView& camera)
{
    // In _drawCharacters
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

void Renderer::_drawEggs(const EggFactory& eggs)
{
    for (const auto& e : eggs.getAll()) {
        Vector3 pos = {
            e.tileX * TILE_SIZE + TILE_SIZE / 2.f,
            0.15f,
            e.tileY * TILE_SIZE + TILE_SIZE / 2.f
        };
        DrawSphere(pos, TILE_SIZE * 0.2f, YELLOW);
    }
}