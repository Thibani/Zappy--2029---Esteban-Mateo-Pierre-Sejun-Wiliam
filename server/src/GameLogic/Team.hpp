#pragma once
#include "Egg.hpp"
#include <vector>

namespace Zappy {

    class Team {

        public:
            Team(std::string name, std::vector<Egg*> eggs);
            void addEgg(Egg *egg);
            std::string getName();
            int getNbEggs();

        private:
            std::vector<Egg*> _eggs;
            std::string _name;
    };
}