#pragma once
#include <vector>
#include <string>

namespace Zappy {
    class Egg;
    class Player;

    class Team {
        public:
            Team(const std::string &name, const std::vector<Egg*> &eggs);

            void addEgg(Egg *egg);
            void removeEgg(const Egg *egg);

            void addPlayer(Player *player);
            void removePlayer(const Player *player);

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