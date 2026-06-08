#pragma once
#include <raylib.h>
#include "../Map/Map.hpp"
#include "../Character/Character.hpp"
#include "../Camera/Camera.hpp"

class Renderer {
public:
    static void drawMap(const Map& map, const Character::CharacterFactory& factory, PlayerView camera, Texture2D charTexture);
};