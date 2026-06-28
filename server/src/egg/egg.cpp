#include "../../include/egg/egg.hpp"

namespace Zappy {

    int Egg::_nextEggId = 0;

    Egg::Egg(const std::string &teamName, Pos position)
        : _id(_nextEggId++), _teamName(teamName), _position(position)
    {
    }
}