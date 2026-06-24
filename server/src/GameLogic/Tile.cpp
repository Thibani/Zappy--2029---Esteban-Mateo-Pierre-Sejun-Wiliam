#include "Tile.hpp"
#include "Resource.hpp"

namespace Zappy {

    void Tile::addPlayer(Player *player)
    {
        _players.push_back(player);
    }

    void Tile::removePlayer(Player *player)
    {
        for (std::vector<Player*>::iterator it = _players.begin(); it != _players.end(); it++) {
            if (*it == player)
            {
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
}