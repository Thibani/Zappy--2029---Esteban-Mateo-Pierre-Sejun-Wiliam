#pragma once
#include <vector>
#include "../types/position.hpp"
#include "../types/resource.hpp"
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
            void addEgg(Egg *egg);
            void removeEgg(Egg* egg);
            std::vector<Player*> getPlayers() { return _players; }
            int getNbPlayers();
            int getNbResources(TypeResource typeResource);
            void resourceConsume(int level);
            const std::vector<int>& resources() const { return _resources; }

        private:
            std::vector<Player*> _players;
            std::vector<Egg*> _eggs;
            std::vector<int> _resources;
            Pos position;
    };
}