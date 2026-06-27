#include "../../include/map/tile.hpp"
#include "../../include/types/resource.hpp"
#include "../../include/egg/egg.hpp"
#include <cstdio>

namespace Zappy {

    void Tile::addPlayer(Player *player)
    {
        _players.push_back(player);
    }

    void Tile::removePlayer(Player *player)
    {
        for (std::vector<Player*>::iterator it = _players.begin(); it != _players.end(); it++) {
            if (*it == player){
                _players.erase(it);
                break;
            }
        }
    }

    void Tile::addResource(TypeResource typeResource)
    {
        _resources[typeResource]++;
    }

    void Tile::removeResource(TypeResource typeResource)
    {
        _resources[typeResource]--;
    }

    bool Tile::hasRessource(TypeResource typeResource)
    {
        return _resources[typeResource] > 0;
    }

    std::string Tile::toString()
    {
        std::string output;

        for (uint nbPlayers = 0; nbPlayers < _players.size(); nbPlayers++)
            output += "player ";
        for (uint nbEggs = 0; nbEggs < _eggs.size(); nbEggs++)
            output += "egg ";
        for (int idxTypeResource = 0; idxTypeResource < (int)_resources.size(); idxTypeResource++)
            for(int nbResources = 0; nbResources < _resources[idxTypeResource]; nbResources++)
                output += Resource::typeResourceToString((TypeResource)idxTypeResource) + " ";
        return output;
    }

    void Tile::addEgg(Egg *egg)
    {
        _eggs.push_back(egg);
    }

    void Tile::removeEgg(Egg* egg)
    {
        for (auto it = _eggs.begin(); it != _eggs.end(); it++) {
            if (*it == egg) {
                _eggs.erase(it);
                return;
            }
        }
    }

    int Tile::getNbPlayers()
    {
        int i = 0;

        while(i < (int)_players.size())
            i++;
        return i;
    }

    int Tile::getNbResources(TypeResource typeResource)
    {
        return _resources[typeResource];
    }

    void Tile::resourceConsume(int level)
    {
        if (level == 1){
            _resources[LINEMATE] -= 1;
        }
        if (level == 2){
            _resources[LINEMATE] -= 1;
            _resources[DERAUMERE] -= 1;
            _resources[SIBUR] -= 1;
        }
        if (level == 3){
            _resources[LINEMATE] -= 2;
            _resources[SIBUR] -= 1;
            _resources[PHIRAS] -= 2;
        }
        if (level == 4){
            _resources[LINEMATE] -= 1;
            _resources[DERAUMERE] -= 1;
            _resources[SIBUR] -= 2;
            _resources[PHIRAS] -= 1;
        }
        if (level == 5){
            _resources[LINEMATE] -= 1;
            _resources[DERAUMERE] -= 2;
            _resources[SIBUR] -= 1;
            _resources[MENDIANE] -= 3;
        }
        if (level == 6){
            _resources[LINEMATE] -= 1;
            _resources[DERAUMERE] -= 2;
            _resources[SIBUR] -= 3;
            _resources[PHIRAS] -= 1;
        }
        if (level == 7){
            _resources[LINEMATE] -= 2;
            _resources[DERAUMERE] -= 2;
            _resources[SIBUR] -= 2;
            _resources[MENDIANE] -= 2;
            _resources[PHIRAS] -= 2;
            _resources[THYSTAME] -= 1;
        }
    }

    void Tile::deleteAllEggs()
    {
        for (Egg *egg : _eggs){
            delete egg;
        }
        _eggs.clear();
    }

}