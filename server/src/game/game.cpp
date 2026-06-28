#include "game/game.hpp"
#include "egg/egg.hpp"
#include "map/map.hpp"
#include "team/team.hpp"
#include "map/tile.hpp"
#include "player/player.hpp"
#include "types/resource.hpp"
#include "utils/clock.hpp"

#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>
#include <iostream>
#include <iterator>
#include <climits>
#include <cmath>

namespace Zappy {


    Game::Game()
        : map(nullptr), _freq(0), _isVictory(false), _spawnResourceDeadline{}
    {
    }

    Game::~Game()
    {
        for (auto &[id, p] : _idPlayers)
            delete p;
        for (Team* t : _teams)
            delete t;
        delete map;
    }

    void Game::initialize(int freq, int mapWidth, int mapHeight, const std::vector<std::string> &teamNames, int nbEggs)
    {
        Pos eggPos;
        std::vector<Egg*> eggs;
        Egg *egg;
        Tile *tile;

        std::srand(std::time(nullptr));
        _freq = freq;
        initActionCost();
        _isVictory = false;
        setNextResourcesDeadline();
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

    void Game::setNextResourcesDeadline()
    {
        _spawnResourceDeadline.deadLine = Clock::deadline(20, _freq);
    }

    void Game::initActionCost()
    {
        _actionCosts[ActionType::BROADCAST] = 7;
        _actionCosts[ActionType::CONNECT_NBR] = 0;
        _actionCosts[ActionType::EAT] = 126;
        _actionCosts[ActionType::EJECT] = 7;
        _actionCosts[ActionType::FORK] = 42;
        _actionCosts[ActionType::FORWARD] = 7;
        _actionCosts[ActionType::INCANTATION] = 300;
        _actionCosts[ActionType::INVENTORY] = 1;
        _actionCosts[ActionType::LEFT] = 7;
        _actionCosts[ActionType::LOOK] = 7;
        _actionCosts[ActionType::RIGHT] = 7;
        _actionCosts[ActionType::SET] = 7;
        _actionCosts[ActionType::TAKE] = 7;
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

    int Game::getTeamNbEggs(const std::string &teamName)
    {
        for (uint i = 0; i < _teams.size(); i++){
            if (teamName == _teams[i]->getName()){
                return _teams[i]->getNbEggs();
            }
        }
        return 0;
    }

    bool Game::eggHatching(int clientId, const std::string &teamName)
    {
        if (hasIdPlayer(clientId))
            return false;
        Team *team = getTeam(teamName);
        if (team == nullptr)
            return false;
        Egg *egg = team->popEgg();
        Player *player = map->eggHatching(egg, teamName);
        team->addPlayer(player);
        _idPlayers[clientId] = player;
        addClientEatAction(clientId);
        if (_listener) {
            Pos p = player->getPosition();
            _listener->onPlayerConnected(clientId, p.x, p.y, player->getDirection(), player->getLevel(), teamName);
        }
        return true;
    }

    std::string Game::moveForward(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->moveForward(map);
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
            return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::turnRight(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnRight();
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
            return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::turnLeft(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            player->turnLeft();
            if (_listener) {
                Pos p = player->getPosition();
                _listener->onPlayerMoved(clientId, p.x, p.y, player->getDirection());
            }
            return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::look(int clientId)
    {
        if (hasIdPlayer(clientId)){
            Player* player = _idPlayers[clientId];
            return player->look(map) + "\n";
        }
        return "[]\n";
    }

    std::string Game::inventory(int clientId)
    {
        if (hasIdPlayer(clientId)){
            std::string output = "[";
            const Player* player = _idPlayers[clientId];
            const std::vector<int> &inv = player->getInventory();
            for (uint typeRes = 0; typeRes < inv.size(); typeRes++){
                output += Resource::typeResourceToString((TypeResource)typeRes);
                output += " ";
                output += std::to_string(inv[typeRes]);
                if (typeRes < inv.size() - 1)
                    output += ",";
            }
            output += "]\n";
            return output;
        }
        return "ko\n";
    }

    std::vector<std::pair<int, std::string>> Game::broadcast(int clientId, const std::string &obj)
    {
        std::vector<std::pair<int, std::string>> idOutputs;
        std::pair<int, std::string> idOutput;

        if (hasIdPlayer(clientId)){
            const Player* player = _idPlayers[clientId];
            for (auto it = _idPlayers.begin(); it != _idPlayers.end(); it++){
                if (player == it->second)
                    continue;
                int pos = computeDirection(player->getPosition(), it->second);
                idOutput.first = it->first;
                idOutput.second = "message " + std::to_string(pos) + ", " + obj + "\n";
                idOutputs.push_back(idOutput);
            }
            idOutput.first = clientId;
            idOutput.second = "ok\n";
            idOutputs.push_back(idOutput);
            if (_listener)
                _listener->onPlayerBroadcast(clientId, obj);
            return idOutputs;
        }
        return idOutputs;
    }

    int Game::computeDirection(Pos emitter, const Player *receiver)
    {
        Pos v = shortestVector(emitter, receiver->getPosition(), map->getWidth(), map->getHeight());

        if (v.x == 0 && v.y == 0)
            return 0;

        v = rotate(v, receiver->getDirection());

        double angle = atan2(-v.y, v.x);

        angle = M_PI / 2 - angle;

        if (angle < 0)
            angle += 2 * M_PI;

        double sector = 2 * M_PI / 8.0;

        int index = (int)round(angle / sector) % 8;

        return index + 1;
    }

    Pos Game::shortestVector(Pos emitter, Pos receiver, int width, int height)
    {
        int dx = emitter.x - receiver.x;
        int dy = emitter.y - receiver.y;

        if (dx > width / 2)
            dx -= width;
        if (dx < -width / 2)
            dx += width;

        if (dy > height / 2)
            dy -= height;
        if (dy < -height / 2)
            dy += height;

        return {dx, dy};
    }

    Pos Game::rotate(Pos v, Direction d)
    {
        switch (d) {

            case Zappy::Direction::UP:
                return v;

            case Zappy::Direction::RIGHT:
                return {-v.y, v.x};

            case Zappy::Direction::DOWN:
                return {-v.x, -v.y};

            case Zappy::Direction::LEFT:
                return {v.y, -v.x};
        }
        return v;
    }

    std::string Game::connectNbr(int clientId)
    {
        if (hasIdPlayer(clientId)){
            const Player* player = _idPlayers[clientId];
            return std::to_string(getTeamNbEggs(player->getTeamName())) + "\n";
        }
        return "ko\n";
    }

    std::string Game::fork(int clientId)
    {
        if (hasIdPlayer(clientId)){
            const Player* player = _idPlayers[clientId];
            Egg *egg = new Egg(player->getTeamName(), player->getPosition());
            getTeam(player->getTeamName())->addEgg(egg);
            Tile* tile = map->getTile(player->getPosition());
            tile->addEgg(egg);
            if (_listener) {
                static int nextEggId = 1;
                int eggId = nextEggId++;
                Pos pos = player->getPosition();
                _listener->onPlayerForked(clientId);
                _listener->onEggLaid(eggId, clientId, pos.x, pos.y);
            }
            return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::eject(int clientId)
    {
        if (hasIdPlayer(clientId)){
            const Player* player = _idPlayers[clientId];
            Tile *tile = map->getTile(player->getPosition());
            std::vector<Player*> players = tile->getPlayers();
            std::vector<Egg*> eggs = tile->getEggs();
            for (const Egg *egg : eggs) {
                Team *team = getTeam(egg->getTeamName());
                if (team)
                    team->removeEgg(egg);
            }
            tile->deleteAllEggs();
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
            return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::incantationStart(int clientId)
    {
        if (hasIdPlayer(clientId)) {
            const Player *player = _idPlayers[clientId];
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
                return "ko\n";
            }
            addClientAction(clientId, Zappy::Game::ActionType::INCANTATION, "");
            return "Elevation underway\n";
            // tile->resourceConsume(oldLevel);
            // player->levelUp();
            // int newLevel = player->getLevel();
            // if (_listener) {
            //     Inventory inv;
            //     const auto &raw = tile->resources();
            //     for (int i = 0; i < 7; i++)
            //         inv.set(static_cast<TypeResource>(i), raw[i]);
            //     _listener->onIncantationEnded(pos.x, pos.y, true, participants, newLevel, inv);
            // }
            // return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::incantationEnd(int clientId)
    {
        if (hasIdPlayer(clientId)) {
            const Player *player = _idPlayers[clientId];
            Tile *tile = map->getTile(player->getPosition());
            Pos pos = player->getPosition();
            int oldLevel = player->getLevel();
            std::vector<int> participants = { clientId };
            if (checkLevelUp(oldLevel, tile)) {
                tile->resourceConsume(oldLevel);
                std::vector<Player*> tilePlayers = tile->getPlayers();
                for (Player* tp : tilePlayers) {
                    if (tp->getLevel() == oldLevel)
                        tp->levelUp();
                }
                int newLevel = player->getLevel();
                if (_listener) {
                    Inventory inv;
                    const auto &raw = tile->resources();
                    for (int i = 0; i < 7; i++)
                        inv.set(static_cast<TypeResource>(i), raw[i]);
                    _listener->onIncantationEnded(pos.x, pos.y, true, participants, newLevel, inv);
                }
                checkWinCondition();
                return "Current level " + std::to_string(newLevel) + "\n";
            }
            if (_listener) {
                Inventory inv;
                const auto &raw = tile->resources();
                for (int i = 0; i < 7 ; i++)
                    inv.set(static_cast<TypeResource>(i), raw[i]);
                _listener->onIncantationEnded(pos.x, pos.y, false, participants, oldLevel, inv);
            }
        }
        return "ko\n";
    }

    std::string Game::take(int clientId, const std::string &obj)
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
            if (ok)
                return "ok\n";
        }
        return "ko\n";
    }

    std::string Game::set(int clientId, const std::string &obj)
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
            if (ok)
                return "ok\n";
        }
        return "ko\n";
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

    Team *Game::getTeam(const std::string &teamName)
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
            const Player* player = _idPlayers[clientId];
            Tile* tile = map->getTile(player->getPosition());
            tile->removePlayer(player);
            _idPlayers.erase(clientId);
            _idActions.erase(clientId);
            _idEatActions.erase(clientId);
            getTeam(player->getTeamName())->removePlayer(player);
            if (_listener)
                _listener->onPlayerDied(clientId);
        }
    }

    bool Game::hasIdPlayer(int clientId)
    {
        auto it = _idPlayers.find(clientId);

        return it != _idPlayers.end();
    }

    bool Game::hasIdAction(int clientId)
    {
        auto it = _idActions.find(clientId);

        return it != _idActions.end();
    }

    void Game::addClientAction(int clientId, ActionType actionType, const std::string &arg)
    {
        Action action;
        if (hasIdAction(clientId) == false){
            action.actionType = actionType;
            action.arg = arg;
            action.deadLine = Clock::deadline(_actionCosts[actionType], _freq);
            _idActions[clientId] = action;
        }
    }

    void Game::addClientEatAction(int clientId)
    {
        Action action;

        if (_isVictory)
            return;

        action.actionType = EAT;
        action.deadLine = Clock::deadline(_actionCosts[EAT], _freq);
        _idEatActions[clientId] = action;
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

    std::vector<std::pair<int, std::string>> Game::executeAllClientActions()
    {
        std::pair<int, std::string> idOutput;
        std::vector<std::pair<int, std::string>> actionResponses;

        for (auto it = _idActions.begin(); it != _idActions.end(); ) {
            if (Clock::hasPassed(it->second.deadLine)) {
                switch (it->second.actionType) {
                    case FORWARD:
                        idOutput.first = it->first;
                        idOutput.second = moveForward(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case LEFT:
                        idOutput.first = it->first;
                        idOutput.second = turnLeft(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case RIGHT:
                        idOutput.first = it->first;
                        idOutput.second = turnRight(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case LOOK:
                        idOutput.first = it->first;
                        idOutput.second = look(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case TAKE:
                        idOutput.first = it->first;
                        idOutput.second = take(it->first, it->second.arg);
                        actionResponses.push_back(idOutput);
                        break;
                    case SET:
                        idOutput.first = it->first;
                        idOutput.second = set(it->first, it->second.arg);
                        actionResponses.push_back(idOutput);
                        break;
                    case EJECT:
                        idOutput.first = it->first;
                        idOutput.second = eject(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case INVENTORY:
                        idOutput.first = it->first;
                        idOutput.second = inventory(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case CONNECT_NBR:
                        idOutput.first = it->first;
                        idOutput.second = connectNbr(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case FORK:
                        idOutput.first = it->first;
                        idOutput.second = fork(it->first);
                        actionResponses.push_back(idOutput);
                        break;
                    case BROADCAST:
                    {
                        auto msgs = broadcast(it->first, it->second.arg);
                        std::copy(msgs.begin(), msgs.end(), std::back_inserter(actionResponses));
                        break;
                    }
                    case INCANTATION:
                    {
                        int initiatorId = it->first;
                        const Player* initiator = _idPlayers[initiatorId];
                        std::string result = incantationEnd(initiatorId);
                        actionResponses.push_back({initiatorId, result});
                        if (result.rfind("Current level", 0) == 0) {
                            Pos pos = initiator->getPosition();
                            for (const auto& [id, p] : _idPlayers) {
                                if (id != initiatorId &&
                                    p->getPosition().x == pos.x &&
                                    p->getPosition().y == pos.y &&
                                    p->getLevel() == initiator->getLevel()) {
                                    actionResponses.push_back({id, result});
                                }
                            }
                        }
                        break;
                    }
                    case EAT:
                        break;
                }
                if (_isVictory)
                    break;
                it = _idActions.erase(it);
            } else {
                ++it;
            }
        }
        return actionResponses;
    }

    std::vector<std::pair<int, std::string>> Game::executeAllEatActions()
    {
        std::vector<std::pair<int, std::string>> actionResponses;
        for (auto it = _idEatActions.begin(); it != _idEatActions.end(); ) {
            if (Clock::hasPassed(it->second.deadLine)) {
                int id = it->first;
                it = _idEatActions.erase(it);
                if (eat(id) == true) {
                    addClientEatAction(id);
                } else {
                    actionResponses.push_back({id, "dead\n"});
                }
            } else {
                ++it;
            }
        }
        return actionResponses;
    }

    void Game::checkWinCondition()
    {
        auto it = std::find_if(_teams.begin(), _teams.end(),
            [](Team *team) { return team->checkWinCondition(); });
        if (it != _teams.end()) {
            const Team *team = *it;
            _isVictory = true;
            _idActions.clear();
            _idEatActions.clear();
            std::cout << "Team " << team->getName() << " is victorious !!! Game is done." << std::endl;
            if (_listener)
                _listener->onGameEnded(team->getName());
        }
    }

    void Game::executeSpawnResources()
    {
        if (_isVictory)
            return;
        if (Clock::hasPassed(_spawnResourceDeadline.deadLine)){
            map->setRessource();
            setNextResourcesDeadline();
            if (_listener) {
                int w = map->getWidth();
                int h = map->getHeight();
                for (int y = 0; y < h; y++) {
                    for (int x = 0; x < w; x++) {
                        Pos p{x, y};
                        const Tile *tile = map->getTile(p);
                        if (!tile)
                            continue;
                        Inventory inv;
                        const auto &raw = tile->resources();
                        for (int i = 0; i < 7; i++)
                            inv.set(static_cast<TypeResource>(i), raw[i]);
                        _listener->onTileChanged(x, y, inv);
                    }
                }
            }
        }
    }

    int Game::nearestDeadlineMs()
    {
        int smallest = INT_MAX;
        for (auto &[id, action] : _idActions) {
            int ms = Clock::msUntil(action.deadLine);
            if (ms < smallest)
                smallest = ms;
        }
        for (auto &[id, action] : _idEatActions) {
            int ms = Clock::msUntil(action.deadLine);
            if (ms < smallest)
                smallest = ms;
        }
        if (!_isVictory) {
            int ms = Clock::msUntil(_spawnResourceDeadline.deadLine);
            if (ms < smallest)
                smallest = ms;
        }
        if (smallest == INT_MAX)
            return -1;
        return smallest > 0 ? smallest : 0;
    }
}