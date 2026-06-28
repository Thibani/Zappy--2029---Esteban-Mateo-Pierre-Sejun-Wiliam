#pragma once
#include <string>
#include <vector>
#include "../types/resource.hpp"
#include "../types/position.hpp"


namespace Zappy {

    class Map;

    enum Direction {
        UP      = 1,
        RIGHT   = 2,
        DOWN    = 3,
        LEFT    = 4
    };

    class Player {

        public:
            explicit Player(const std::string &teamName);
            ~Player() = default;
            int getLevel() const { return _level; }
            Direction getDirection() const { return _direction; }
            Pos getPosition() const { return _position; }
            const std::string &getTeamName() const { return _teamName; }
            const std::vector<int> &getInventory() const { return _inventory; }

            void setTeamName(const std::string &teamName) { _teamName = teamName; }
            void setPosition(Pos position) { _position = position; }

            void turnLeft();
            void turnRight();
            void moveForward(Map* map);
            bool take(Map* map, TypeResource typeResource);
            bool drop(Map* map, TypeResource typeResource);
            bool eat();
            std::string look(Map* map);
            void getEject(Map *map, Direction direction);
            void levelUp() { _level++; };

        private:
            int _level;
            Direction _direction;
            Pos _position;
            std::string _teamName;
            std::vector<int>_inventory;

            std::vector<Pos> lookUpVision(Map* map);
            std::vector<Pos> lookDownVision(Map* map);
            std::vector<Pos> lookRightVision(Map* map);
            std::vector<Pos> lookLeftVision(Map* map);
            static std::vector<Pos> lookUp(Map* map, Pos startPos, int distance);
            static std::vector<Pos> lookDown(Map* map, Pos startPos, int distance);
            static std::vector<Pos> lookRight(Map* map, Pos startPos, int distance);
            static std::vector<Pos> lookLeft(Map* map, Pos startPos, int distance);
            static std::string tilesToString(Map* map, const std::vector<Pos> &vision);
    };
}