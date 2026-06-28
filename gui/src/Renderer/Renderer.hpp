#pragma once
#include <raylib.h>
#include "../Map/Map.hpp"
#include "../Character/CharacterFactory.hpp"
#include "../Egg/EggFactory.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/AssetManager.hpp"

class Renderer {
public:
    explicit Renderer(AssetManager& assets);
    void drawMap(const Map& map, const CharacterFactory& factory, const EggFactory& eggs, PlayerView& camera);
private:
    AssetManager& _assets;
    void _drawTiles(const Map& map);
    void _drawCharacters(const CharacterFactory& factory, PlayerView& camera);
    void _drawEggs(const EggFactory& eggs, PlayerView& camera);
};