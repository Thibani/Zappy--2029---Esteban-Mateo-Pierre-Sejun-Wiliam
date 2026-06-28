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
    struct Offset { float dx; float dz; };
    static const Offset offsets[7] = {
        { -0.25f, -0.25f },
        {  0.25f, -0.25f },
        { -0.25f,  0.25f },
        {  0.25f,  0.25f },
        {  0.0f,  -0.25f },
        { -0.25f,  0.0f  },
        {  0.25f,  0.0f  },
    };
    static const char* resKeys[7] = {
        "food", "GoldOre", "IronOre", "Stone", "Fang", "Ruby", "Starshard"
    };

    for (int y = 0; y < map.getHeight(); y++) {
        for (int x = 0; x < map.getWidth(); x++) {
            const Tile& tile = map.getTile(x, y);
            Vector3 pos = {
                x * TILE_SIZE + TILE_SIZE / 2.f,
                0.f,
                y * TILE_SIZE + TILE_SIZE / 2.f
            };

            DrawCube(pos, TILE_SIZE, 0.1f, TILE_SIZE, DARKGREEN);
            DrawCubeWires(pos, TILE_SIZE, 0.1f, TILE_SIZE, BLACK);

            const int quantities[7] = {
                tile.q0, tile.q1, tile.q2, tile.q3, tile.q4, tile.q5, tile.q6
            };

            for (int i = 0; i < 7; i++) {
                if (quantities[i] > 0) {
                    Vector3 resPos = {
                        pos.x + offsets[i].dx,
                        0.15f,
                        pos.z + offsets[i].dz
                    };
                    DrawBillboard(camera.get(), _assets.get(resKeys[i]), resPos, TILE_SIZE * 0.3f, WHITE);
                }
            }
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
        DrawBillboard(camera.get(), _assets.get("egg"), pos, TILE_SIZE * 0.6f, WHITE);
    }
}