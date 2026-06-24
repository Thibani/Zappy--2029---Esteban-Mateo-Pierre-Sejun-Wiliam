#pragma once
#include "Egg.hpp"
#include <vector>

namespace Zappy {

    class Team {

        public:
            Team(std::string);

        private:
            std::vector<Egg*> _eggs;
            std::string _teamName;
    };
}