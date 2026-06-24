#include "Team.hpp"
#include "Egg.hpp"
#include <vector>

namespace Zappy {

    Team::Team(std::string name, std::vector<Egg*> eggs)
    {
        _name = name;
        _eggs = eggs;
    }

    void Team::addEgg(Egg *egg)
    {
        _eggs.push_back(egg);
    }

    std::string Team::getName()
    {
        return _name;
    }

    int Team::getNbEggs()
    {
        return _eggs.size();
    }
}
