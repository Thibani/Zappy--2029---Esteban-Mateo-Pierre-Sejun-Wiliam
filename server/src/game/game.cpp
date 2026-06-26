#include "game/game.hpp"
#include "egg/egg.hpp"
#include "map/map.hpp"
#include "team/team.hpp"
#include "map/tile.hpp"
#include "player/player.hpp"
#include "types/resource.hpp"

#include <cstdlib>
#include <random>
#include <vector>
#include <iostream>
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
        map->setRessource();
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

    bool Game::eggHatching(int clientId, const std::string teamName)
    {
        std::cout << "[eggHatching] called id=" << clientId << " team=" << teamName << " listener=" << (_listener?"yes":"no") << "\n";
        if (hasIdPlayer(clientId))
            return false;
        Team *team = getTeam(teamName);
        if (team == nullptr)
            return false;
        Egg *egg = team->popEgg();
        Player *player = map->eggHatching(egg, teamName);
        _idPlayers[clientId] = player;
        if (_listener) {
            Pos p = player->getPosition();
            _listener->onPlayerConnected(clientId, p.x, p.y, player->getDirection(), player->getLevel(), teamName);
        }
        return true;
    }

    void Game::moveForward(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->moveForward(map);
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
        }
    }

    void Game::turnRight(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnRight();
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
        }
    }

    void Game::turnLeft(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnLeft();
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
        }
    }

    std::string Game::look(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return player->look(map);
        }
        return "[]\n";
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
            Pos pos = player->getPosition();
            Egg *egg = new Egg(player->getTeamName(), player->getPosition());
            getTeam(player->getTeamName())->addEgg(egg);
            Tile* tile = map->getTile(player->getPosition());
            tile->addEgg(egg);
            if (_listener) {
                static int nextEggId = 1;
                int eggId = nextEggId++;
                _listener->onPlayerForked(clientId);
                _listener->onEggLaid(eggId, clientId, pos.x, pos.y);
            }
            return getTeamNbEggs(player->getTeamName());
        }
        return 0;
    }

    void Game::eject(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            Tile *tile = map->getTile(player->getPosition());
            std::vector<Player*> players = tile->getPlayers();
            for (uint i = 0; i < players.size(); i++){
                if (player == players[i])
                    continue;
                players[i]->getEject(map, player->getDirection());
                if (_listener) {
                    for (auto &[id, p] : _idPlayers) {
                        if (p == players[i]) {
                            _listener->onPlayerExpelled(id);
                            break;
                        }
                    }
                }
            }
        }
    }

    bool Game::incantation(int clientId)
    {
        if (hasIdPlayer(clientId)) {
            Player *player = _idPlayers[clientId];
            Tile *tile = map->getTile(player->getPosition());
            Pos pos = player->getPosition();
            int oldLevel = player->getLevel();
            //TODO: Add all players on the tile to participants
            std::vector<int> participants = { clientId };
            if (_listener)
                _listener->onIncantationStarted(pos.x, pos.y, oldLevel, participants);
            if (checkLevelUp(oldLevel, tile) == false) {
                if (_listener) {
                    Inventory inv;
                    const auto &raw = tile->resources();
                    for (int i = 0; i < 7; i++)
                        inv.set(static_cast<TypeResource>(i), raw[i]);
                    _listener->onIncantationEnded(pos.x, pos.y, false, participants, oldLevel, inv);
                }
                return false;
            }
            tile->resourceConsume(oldLevel);
            player->levelUp();
            int newLevel = player->getLevel();
            if (_listener) {
                Inventory inv;
                const auto &raw = tile->resources();
                for (int i = 0; i < 7; i++)
                    inv.set(static_cast<TypeResource>(i), raw[i]);
                _listener->onIncantationEnded(pos.x, pos.y, true, participants, newLevel, inv);
            }
            return true;
        }
        return false;
    }

    bool Game::take(int clientId, const std::string obj)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            TypeResource r = Resource::stringToTypeResource(obj);
            bool ok = player->take(map, r);
            if (ok && _listener) {
                Pos p = player->getPosition();
                Inventory tileInv;
                Inventory playerInv;
                const auto &tileRaw = map->getTile(p)->resources();
                const auto &playerRaw = player->getInventory();
                for (int i = 0; i < 7; i++) {
                    tileInv.set(static_cast<TypeResource>(i), tileRaw[i]);
                    playerInv.set(static_cast<TypeResource>(i), playerRaw[i]);
                }
                _listener->onPlayerTookResource(clientId, r, p.x, p.y, tileInv, playerInv);
            }
            return ok;
        }
        return false;
    }

    bool Game::set(int clientId, const std::string obj)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            TypeResource r = Resource::stringToTypeResource(obj);
            bool ok = player->drop(map, r);
            if (ok && _listener) {
                Pos p = player->getPosition();
                Inventory tileInv;
                Inventory playerInv;
                const auto &tileRaw = map->getTile(p)->resources();
                const auto &playerRaw = player->getInventory();
                for (int i = 0; i < 7; i++) {
                    tileInv.set(static_cast<TypeResource>(i), tileRaw[i]);
                    playerInv.set(static_cast<TypeResource>(i), playerRaw[i]);
                }
                _listener->onPlayerDroppedResource(clientId, r, p.x, p.y, tileInv, playerInv);
            }
            return ok;
        }
        return false;
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
            _idPlayers.erase(clientId);
            if (_listener)
                _listener->onPlayerDied(clientId);
        }
    }

    bool Game::hasIdPlayer(int clientId)
    {
        auto it = _idPlayers.find(clientId);

        return it != _idPlayers.end();
    }

    bool Game::checkLevelUp(int level, Tile* tile)
    {
        if (level == 1){
            if (tile->getNbPlayers() >= 1){
                if (tile->getNbResources(LINEMATE) >= 1)
                    return true;
            }
        }
        if (level == 2){
            if (tile->getNbPlayers() >= 2){
                if (tile->getNbResources(LINEMATE) >= 1
                && tile->getNbResources(DERAUMERE) >= 1
                && tile->getNbResources(SIBUR) >= 1)
                    return true;
            }
        }
        if (level == 3){
            if (tile->getNbPlayers() >= 2){
                if (tile->getNbResources(LINEMATE) >= 2
                && tile->getNbResources(SIBUR) >= 1
                && tile->getNbResources(PHIRAS) >= 2)
                    return true;
            }
        }
        if (level == 4){
            if (tile->getNbPlayers() >= 4){
                if (tile->getNbResources(LINEMATE) >= 1
                && tile->getNbResources(DERAUMERE) >= 1
                && tile->getNbResources(SIBUR) >= 2
                && tile->getNbResources(PHIRAS) >= 1)
                    return true;
            }
        }
        if (level == 5){
            if (tile->getNbPlayers() >= 4){
                if (tile->getNbResources(LINEMATE) >= 1
                && tile->getNbResources(DERAUMERE) >= 2
                && tile->getNbResources(SIBUR) >= 1
                && tile->getNbResources(MENDIANE) >= 3)
                    return true;
            }
        }
        if (level == 6){
            if (tile->getNbPlayers() >= 6){
                if (tile->getNbResources(LINEMATE) >= 1
                && tile->getNbResources(DERAUMERE) >= 2
                && tile->getNbResources(SIBUR) >= 3
                && tile->getNbResources(PHIRAS) >= 1)
                    return true;
            }
        }
        if (level == 7){
            if (tile->getNbPlayers() >= 6){
                if (tile->getNbResources(LINEMATE) >= 2
                && tile->getNbResources(DERAUMERE) >= 2
                && tile->getNbResources(SIBUR) >= 2
                && tile->getNbResources(MENDIANE) >= 2
                && tile->getNbResources(PHIRAS) >= 2
                && tile->getNbResources(THYSTAME) >= 1)
                    return true;
            }
        }
        return false;
    }
}