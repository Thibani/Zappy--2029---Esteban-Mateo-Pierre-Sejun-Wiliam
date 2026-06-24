#pragma once
#include <string>
#include <vector>
#include "Resource.hpp"
#include "Position.hpp"


namespace Zappy {

    class Map;

    enum Direction {
        UP,
        DOWN,
        RIGHT,
        LEFT
    };

    class Player {

        public:
            Player(std::string teamName);
            ~Player() = default;
            int getLevel() { return _level; }
            Direction getDirection() { return _direction; }
            Pos getPosition() { return _position; }
            int getFoodLevel() { return _foodLevel; }
            std::string getTeamName() { return _teamName; }
            std::vector<int> getInventory() { return _inventory; }

            void setTeamName(std::string teamName) { _teamName = teamName; }

            void turnLeft();
            void turnRight();
            void moveForward(Map* map);
            bool take(Map* map, TypeResource typeResource);
            bool drop(Map* map, TypeResource typeResource);
            bool hungry();
            bool eat();
            void checkLimit(Pos position);
            std::string look(Map* map);

            int _level;

        private:
            // int _level;
            Direction _direction;
            Pos _position;
            int _foodLevel;
            std::string _teamName;
            std::vector<int>_inventory;

            std::vector<Pos> lookUpVision(Map* map);
            std::vector<Pos> lookDownVision(Map* map);
            std::vector<Pos> lookRightVision(Map* map);
            std::vector<Pos> lookLeftVision(Map* map);
            std::vector<Pos> lookUp(Map* map, Pos startPos, int distance);
            std::vector<Pos> lookDown(Map* map, Pos startPos, int distance);
            std::vector<Pos> lookRight(Map* map, Pos startPos, int distance);
            std::vector<Pos> lookLeft(Map* map, Pos startPos, int distance);
            std::string tilesToString(Map* map, std::vector<Pos> vision);
    };
}