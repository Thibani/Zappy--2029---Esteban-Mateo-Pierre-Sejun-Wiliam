#pragma once
#include <vector>
#include "types/position.hpp"
#include "types/resource.hpp"
#include <string>

namespace Zappy {

    class Player;
    class Egg;

    class Tile {
        public:
            Tile() : _resources(7) {}

            void addPlayer(Player *player);
            void removePlayer(Player *player);
            void addResource(TypeResource typeResource);
            void removeResource(TypeResource typeResource);
            bool hasRessource(TypeResource typeResource);
            std::string toString();

            const std::vector<int> &resources() const { return _resources; }
            const std::vector<Player *> &players() const { return _players; }
            const std::vector<Egg *> &eggs() const { return _eggs; }

        private:
            std::vector<Player*> _players;
            std::vector<Egg*> _eggs;
            std::vector<int> _resources;

        public:
            Pos position;
    };
}