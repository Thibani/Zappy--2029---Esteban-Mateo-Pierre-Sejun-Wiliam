#include "../../include/team/team.hpp"
#include "../../include/egg/egg.hpp"
#include "../../include/player/player.hpp"
#include <vector>

namespace Zappy {

    Team::Team(std::string name, std::vector<Egg*> eggs)
    {
        _name = name;
        _eggs = eggs;
    }

    void Team::addPlayer(Player *player)
    {
        _players.push_back(player);
    }

    void Team::removePlayer(Player *playerToRemove)
    {
        for (std::vector<Player*>::iterator it = _players.begin(); it != _players.end(); it++){
            if (playerToRemove == *it){
                _players.erase(it);
                break;
            }
        }
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

    bool Team::checkWinCondition()
    {
        int nbPlayerLevelMax = 0;

        for (Player *player : _players){
            if (player->getLevel() >= 8)
                nbPlayerLevelMax++;
        }
        return nbPlayerLevelMax >= 6;
    }
}
