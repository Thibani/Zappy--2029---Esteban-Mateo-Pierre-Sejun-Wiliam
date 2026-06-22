#pragma once
#include "Ressource.hpp"
#include "Player.hpp"

struct Egg {
    std::string teamName;
};

struct Case {
    Case() : ressource(7) {}
    std::vector<Player*> player;
    std::vector<Egg*> egg;
    std::vector<int> ressource;
};

class Map {

    public:
        Map(int height, int width);
        ~Map() = default;
        Case& getCase(int x, int y);
        void setRessource();
        int getHeight() { return _height; }
        int getWidth() { return _width; }

    private:
        std::vector<std::vector<Case>> _case;
        int _height;
        int _width;
        float _densities[7] = {0.5, 0.3, 0.15, 0.1, 0.1, 0.08, 0.05};
};
