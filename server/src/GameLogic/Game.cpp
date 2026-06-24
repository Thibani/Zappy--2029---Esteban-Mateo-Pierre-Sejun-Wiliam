#include "Game.hpp"
#include "Egg.hpp"
#include "Map.hpp"
#include "Team.hpp"
#include "Tile.hpp"
#include <cstdlib>
#include <random>
#include <vector>

namespace Zappy {


    Game::Game()
    {

    }

    Game::~Game()
    {
        for (Egg* e : _eggs)
            delete e;

        for (Player* p : _players)
            delete p;

        delete map;
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

    int Game::getTeamNbEggs(std::string teamName)
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

    void Game::moveForward(int clientId)
    {
        clientId = 8;
    }

    void Game::turnRight(int clientId)
    {
        clientId = 8;
    }

    void Game::turnLeft(int clientId)
    {
        clientId = 8;
    }

    void Game::look(int clientId)
    {
        clientId = 8;
    }

    void Game::inventory(int clientId)
    {
        clientId = 8;
    }

    // void Game::broadcast(int clientId, const std::string obj)
    // {

    // }

    void Game::connectNbr(int clientId)
    {
        clientId = 8;
    }

    void Game::fork(int clientId)
    {
        clientId = 8;
    }

    void Game::eject(int clientId)
    {
        clientId = 8;
    }

    void Game::incantation(int clientId)
    {
        clientId = 8;
    }

    void Game::take(int clientId, const std::string obj)
    {
        clientId = 8;
    }

    void Game::set(int clientId, const std::string obj)
    {
        clientId = 8;
    }


    // void Game::createTeam(std::string teamName)
    // {
    //     Team team = Team("teamName");

    //     _teams.push_back(&team);
    // }
}