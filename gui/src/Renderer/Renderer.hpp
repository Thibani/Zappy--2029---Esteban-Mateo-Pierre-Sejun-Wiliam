#pragma once
#include <raylib.h>
#include "../Map/Map.hpp"
#include "../Character/CharacterFactory.hpp"
#include "../Camera/Camera.hpp"
#include "../Core/Window.hpp"
#include "../Egg/EggFactory.hpp"

class Renderer {
public:
    explicit Renderer(const char* charTexturePath);
    void drawMap(const Map& map, const CharacterFactory& factory, const EggFactory& eggs, PlayerView& camera);
private:
    ManagedTexture _charTexture;
    void _drawTiles(const Map& map);
    void _drawCharacters(const CharacterFactory& factory, PlayerView& camera);
    void _drawEggs(const EggFactory& eggs);
};