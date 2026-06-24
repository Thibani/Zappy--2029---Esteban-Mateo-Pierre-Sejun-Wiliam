#pragma once

#include <string>
#include "../types/position.hpp"

namespace Zappy {

    class Egg {
        public:
            Egg(std::string teamName, Pos position);
            Pos getPosition() { return _position; }

        private:
            std::string _teamName;
            Pos _position;
    };
}