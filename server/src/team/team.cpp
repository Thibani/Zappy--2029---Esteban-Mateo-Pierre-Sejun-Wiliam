#include "../../include/team/team.hpp"
#include "../../include/egg/egg.hpp"
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

    Egg* Team::popEgg()
    {
        Egg *egg = _eggs.back();
        _eggs.pop_back();
        return egg;
    }
}
