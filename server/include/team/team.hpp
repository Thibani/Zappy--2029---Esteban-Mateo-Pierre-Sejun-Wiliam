#pragma once
#include <vector>
#include <string>

namespace Zappy {

    class Egg;

    class Team {

        public:
            Team(std::string name, std::vector<Egg*> eggs);
            void addEgg(Egg *egg);
            std::string getName();
            int getNbEggs();
            Egg* popEgg();
            const std::vector<Egg*> &getEggs() const { return _eggs; }
        private:
            std::vector<Egg*> _eggs;
            std::string _name;
    };
}