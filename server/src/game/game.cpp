#include "../../include/game/game.hpp"
#include "../../include/egg/egg.hpp"
#include "../../include/map/map.hpp"
#include "../../include/team/team.hpp"
#include "../../include/map/tile.hpp"
#include "../../include/player/player.hpp"
#include "../../include/types/resource.hpp"
#include <cstdlib>
#include <random>
#include <vector>

namespace Zappy {


    Game::Game()
    {

    }

    Game::~Game()
    {
        for (Player* p : _players)
            delete p;
    }

    void Game::initialize(int mapWidth, int mapHeight, std::vector<std::string> teamNames, int nbEggs)
    {
        Pos eggPos;
        std::vector<Egg*> eggs;
        Egg *egg;
        Tile *tile;

        map = new Map(mapWidth, mapHeight);
        for (uint i = 0; i < teamNames.size(); i++){
            eggs.clear();
            for (uint j = 0; j < nbEggs; j++){
                eggPos.x = rand() % mapWidth;
                eggPos.y = rand() % mapHeight;
                egg = new Egg(teamNames[i], eggPos);
                eggs.push_back(egg);
                tile = map->getTile(eggPos);
                tile->addEgg(egg);
            }
            _teams.push_back(new Team(teamNames[i], eggs));
        }
    }

    std::vector<std::string> Game::getTeams()
    {
        std::vector<std::string> teamNames;

        for (uint i = 0; i < _teams.size(); i++)
            teamNames.push_back(_teams[i]->getName());
        return teamNames;
    }

    std::pair<int, int> Game::getMapSize()
    {
        std::pair<int, int> sizeMap;

        sizeMap.first = map->getWidth();
        sizeMap.second = map->getHeight();
        return sizeMap;
    }

    int Game::getTeamNbEggs(const std::string teamName)
    {
        for (uint i = 0; i < _teams.size(); i++){
            if (teamName == _teams[i]->getName()){
                return _teams[i]->getNbEggs();
            }
        }
        return 0;
    }

    void Game::addPlayer(Player* player)
    {
        _players.push_back(player);
        map->addPlayerOnTile(player);
    }

    void Game::eggHatching(int clientId, const std::string teamName)
    {
        if (hasIdPlayer(clientId))
            return;
        Team *team = getTeam(teamName);
        if (team == nullptr)
            return;
        Egg *egg = team->popEgg();
        Player *player = map->eggHatching(egg, teamName);
        _idPlayers[clientId] = player;
    }

    void Game::moveForward(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->moveForward(map);
        }
    }

    void Game::turnRight(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnRight();
        }
    }

    void Game::turnLeft(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnLeft();
        }
    }

    std::string Game::look(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return player->look(map);
        }
    }

    std::string Game::inventory(int clientId)
    {
        std::string output = "[";

        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            std::vector<int> inventory = player->getInventory();
            for (uint typeRes = 0; typeRes < inventory.size(); typeRes++){
                output += Resource::typeResourceToString((TypeResource)typeRes);
                output += " ";
                output += std::to_string(inventory[typeRes]);
                if (typeRes < inventory.size() - 1)
                    output += ",";
            }
            return output += "]";
        }
        return "";
    }

    // void Game::broadcast(int clientId, const std::string obj)
    // {

    // }

    int Game::connectNbr(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return getTeamNbEggs(player->getTeamName());
        }
        return 0;
    }

    int Game::fork(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            Egg *egg = new Egg(player->getTeamName(), player->getPosition());
            getTeam(player->getTeamName())->addEgg(egg);
            Tile* tile = map->getTile(player->getPosition());
            tile->addEgg(egg);
            return getTeamNbEggs(player->getTeamName());
        }
        return 0;
    }

    void Game::eject(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];

        }
    }

    void Game::incantation(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];

        }
    }

    bool Game::take(int clientId, const std::string obj)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return player->take(map, Resource::stringToTypeResource((obj)));
        }
        return false;//throw peut être
    }

    bool Game::set(int clientId, const std::string obj)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return player->drop(map, Resource::stringToTypeResource(obj));
        }
        return false;//throw peut être
    }

    bool Game::eat(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            bool isAlive = player->eat();
            if (isAlive == false)
                removePlayer(clientId);
            return isAlive;
        }
        return false;//throw peut être
    }

    Team *Game::getTeam(const std::string teamName)
    {
        for (uint i = 0; i < _teams.size(); i++){
            if (teamName == _teams[i]->getName()){
                return _teams[i];
            }
        }
        return nullptr;
    }

    void Game::removePlayer(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            Tile* tile = map->getTile(player->getPosition());
            tile->removePlayer(player);
        }
    }

    bool Game::hasIdPlayer(int clientId)
    {
        auto it = _idPlayers.find(clientId);

        return it != _idPlayers.end();
    }
}