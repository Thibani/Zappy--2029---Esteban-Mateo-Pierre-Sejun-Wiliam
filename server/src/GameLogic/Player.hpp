#include "Map.hpp"

enum Direction {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

class Player {

    public:
        Player() : _inventory(7){};
        ~Player() = default;
        int getLevel() { return _level; }
        Direction getDirection() { return _direction; }
        int getPositionX() { return _positionX; }
        int getPositionY() { return _positionY; }
        int getFoodLevel() { return _foodLevel; }
        std::string getTeamName() { return _teamName; }
        std::vector<int> getInventory() { return _inventory; }

    private:
        int _level;
        Direction _direction;
        int _positionX;
        int _positionY;
        int _foodLevel;
        std::string _teamName;
        std::vector<int>_inventory;
};
