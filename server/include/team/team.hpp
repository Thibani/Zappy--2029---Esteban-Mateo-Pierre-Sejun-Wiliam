#pragma once
#include <vector>
#include <string>

namespace Zappy {

    class Egg;
    class Player;

    class Team {

        public:
            Team(std::string name, std::vector<Egg*> eggs);
            void addEgg(Egg *egg);
            void removeEgg(Egg *egg);
            void addPlayer(Player *player);
            void removePlayer(Player *player);
            std::string getName();
            int getNbEggs();
            Egg* popEgg();
            const std::vector<Egg*> &getEggs() const { return _eggs; }
            bool checkWinCondition();
        private:
            std::vector<Egg*> _eggs;
            std::vector<Player*> _players;
            std::string _name;
    };
}