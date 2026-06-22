#pragma once
#include <string>
#include <vector>
#include "Ressource.hpp"
class Map;

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

class Player {

    public:
        Player() : _level(1), _direction(UP), _positionX(0), _positionY(0), _foodLevel(10), _inventory(7){};
        ~Player() = default;
        int getLevel() { return _level; }
        Direction getDirection() { return _direction; }
        int getPositionX() { return _positionX; }
        int getPositionY() { return _positionY; }
        int getFoodLevel() { return _foodLevel; }
        std::string getTeamName() { return _teamName; }
        std::vector<int> getInventory() { return _inventory; }

        void setTeamName(std::string teamName) { _teamName = teamName; }

        void turnLeft();
        void turnRight();
        void moveForward(int mapWidth, int mapHeight);
        bool take(Map& map, Ressource ressource);
        bool set(Map& map, Ressource ressource);
        bool hungry();
        bool eat();

    private:
        int _level;
        Direction _direction;
        int _positionX;
        int _positionY;
        int _foodLevel;
        std::string _teamName;
        std::vector<int>_inventory;
};
