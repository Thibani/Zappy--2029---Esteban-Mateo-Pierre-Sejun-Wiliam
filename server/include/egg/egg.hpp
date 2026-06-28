#pragma once

#include <string>
#include "../types/position.hpp"

namespace Zappy {

    class Egg {
        public:
            Egg(const std::string &teamName, Pos position);
            Pos getPosition() const { return _position; }
            const std::string &getTeamName() const { return _teamName; }
            int getId() { return _id; }

        private:
            static int _nextEggId;
            int _id;
            std::string _teamName;
            Pos _position;
    };
}