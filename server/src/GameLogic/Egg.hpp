#pragma once
#include <string>
#include "Position.hpp"

namespace Zappy {

    class Egg {
        public:
            Egg(std::string teamName, Pos position);

        //private:
            std::string _teamName;
            Pos _position;
    };
}