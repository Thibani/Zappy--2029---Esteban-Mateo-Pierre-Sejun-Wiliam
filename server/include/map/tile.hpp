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
            Tile() : _resources(7), position{} {}

            void addPlayer(Player *player);
            void removePlayer(const Player *player);
            void addResource(TypeResource typeResource);
            void removeResource(TypeResource typeResource);
            bool hasRessource(TypeResource typeResource);
            std::string toString();
            void addEgg(Egg *egg);
            void removeEgg(const Egg* egg);
            const std::vector<Player*> &getPlayers() const { return _players; }
            const std::vector<Egg*> &getEggs() const { return _eggs; }
            int getNbPlayers();
            int getNbResources(TypeResource typeResource);
            void resourceConsume(int level);
            const std::vector<int>& resources() const { return _resources; }
            void deleteAllEggs();

        private:
            std::vector<Player*> _players;
            std::vector<Egg*> _eggs;
            std::vector<int> _resources;
            Pos position;
    };
}