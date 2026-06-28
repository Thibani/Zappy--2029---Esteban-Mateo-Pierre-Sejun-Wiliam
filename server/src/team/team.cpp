#include "team/team.hpp"
#include "egg/egg.hpp"
#include "player/player.hpp"

#include <vector>
#include <algorithm>

namespace Zappy {
    Team::Team(const std::string &name, const std::vector<Egg*> &eggs)
        : _eggs(eggs), _name(name)
    {
    }

    void Team::addPlayer(Player *player)
    {
        _players.push_back(player);
    }

    void Team::removePlayer(const Player *playerToRemove)
    {
        auto it = std::find(_players.begin(), _players.end(), playerToRemove);
        if (it != _players.end())
            _players.erase(it);
    }

    void Team::addEgg(Egg *egg)
    {
        _eggs.push_back(egg);
    }

    void Team::removeEgg(const Egg *egg)
    {
        auto it = std::find(_eggs.begin(), _eggs.end(), egg);
        if (it != _eggs.end())
            _eggs.erase(it);
    }

    const std::string &Team::getName() const
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

    bool Team::checkWinCondition()
    {
        int nbPlayerLevelMax = std::count_if(_players.begin(), _players.end(),
            [](const Player *p) { return p->getLevel() >= 8; });
        return nbPlayerLevelMax >= 6;
    }
}