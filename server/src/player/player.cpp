#include "../../include/player/player.hpp"
#include "../../include/map/map.hpp"
#include "../../include/map/tile.hpp"
#include <iostream>

namespace Zappy {

    Player::Player(const std::string &teamName)
        : _level(1), _direction(static_cast<Direction>(rand() % 4)),
          _position{0, 0}, _teamName(teamName), _inventory(7), _ritualId(-1)
    {
        _inventory[FOOD] = 10;
    }

    void Player::turnLeft()
    {
        switch(_direction) {
            case UP:
                _direction = LEFT;
                break;
            case LEFT:
                _direction = DOWN;
                break;
            case DOWN:
                _direction = RIGHT;
                break;
            case RIGHT:
                _direction = UP;
                break;
        }
    }

    void Player::turnRight()
    {
        switch(_direction) {
            case UP:
                _direction = RIGHT;
                break;
            case RIGHT:
                _direction = DOWN;
                break;
            case DOWN:
                _direction = LEFT;
                break;
            case LEFT:
                _direction = UP;
                break;
        }
    }

    void Player::moveForward(Map* map)
    {
        Tile* tile = map->getTile(_position);
        tile->removePlayer(this);

        switch(_direction) {
            case UP:
                _position.y--;
                break;
            case RIGHT:
                _position.x++;
                break;
            case LEFT:
                _position.x--;
                break;
            case DOWN:
                _position.y++;
                break;
        }

        map->correctPos(&_position);
        tile = map->getTile(_position);
        tile->addPlayer(this);
    }

    bool Player::take(Map* map, TypeResource typeResource)
    {
        Tile* tile = map->getTile(_position);
        if (tile->hasRessource(typeResource)){
            tile->removeResource(typeResource);
            _inventory[typeResource]++;
            return true;
        }
        return false;
    }

    bool Player::drop(Map* map, TypeResource typeResource)
    {
        Tile* tile = map->getTile(_position);
        if (_inventory[typeResource] > 0){
            _inventory[typeResource]--;
            tile->addResource(typeResource);
            return true;
        }
        return false;
    }

    bool Player::eat()
    {
        if (_inventory[FOOD] <= 0)
            return false;
        _inventory[FOOD]--;
        return true;
    }

    void Player::getEject(Map *map, Direction direction)
    {
        Tile* tile = map->getTile(_position);
        tile->removePlayer(this);

        switch(direction) {
            case UP:
                _position.y--;
                break;
            case RIGHT:
                _position.x++;
                break;
            case LEFT:
                _position.x--;
                break;
            case DOWN:
                _position.y++;
                break;
        }

        map->correctPos(&_position);
        tile = map->getTile(_position);
        tile->addPlayer(this);
    }

    std::string Player::look(Map* map)
    {
        std::vector<Pos> vision;

        switch (_direction){
            case UP:
                vision = lookUpVision(map);
                break;
            case DOWN:
                vision = lookDownVision(map);
                break;
            case RIGHT:
                vision = lookRightVision(map);
                break;
            case LEFT:
                vision = lookLeftVision(map);
                break;
        }
        vision.insert(vision.begin(), _position);
        return tilesToString(map, vision);
    }

    std::string Player::tilesToString(Map* map, const std::vector<Pos> &vision)
    {
        std::string output = "[";

        for (uint i = 0; i < vision.size(); i++){
            output += map->getTile(vision[i])->toString();
            if (i < vision.size() - 1)
                output += ",";
        }
        output += "]";
        return output;
    }

    std::vector<Pos> Player::lookUpVision(Map* map)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= _level; i++){
            lookPos.x = _position.x;
            lookPos.y = _position.y - i;
            map->correctPos(&lookPos);
            std::vector<Pos> result = lookLeft(map, lookPos, i);
            pos.insert(pos.end(), result.rbegin(), result.rend());
            pos.push_back(lookPos);
            result = lookRight(map, lookPos, i);
            pos.insert(pos.end(), result.begin(), result.end());
        }
        return pos;
    }

    std::vector<Pos> Player::lookDownVision(Map* map)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= _level; i++){
            lookPos.x = _position.x;
            lookPos.y = _position.y + i;
            map->correctPos(&lookPos);
            std::vector<Pos> result = lookRight(map, lookPos, i);
            pos.insert(pos.end(), result.rbegin(), result.rend());
            pos.push_back(lookPos);
            result = lookLeft(map, lookPos, i);
            pos.insert(pos.end(), result.begin(), result.end());
        }
        return pos;
    }
     std::vector<Pos> Player::lookRightVision(Map* map)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= _level; i++){
            lookPos.x = _position.x + i;
            lookPos.y = _position.y;
            map->correctPos(&lookPos);
            std::vector<Pos> result = lookUp(map, lookPos, i);
            pos.insert(pos.end(), result.rbegin(), result.rend());
            pos.push_back(lookPos);
            result = lookDown(map, lookPos, i);
            pos.insert(pos.end(), result.begin(), result.end());
        }
        return pos;
    }
     std::vector<Pos> Player::lookLeftVision(Map* map)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= _level; i++){
            lookPos.x = _position.x - i;
            lookPos.y = _position.y;
            map->correctPos(&lookPos);
            std::vector<Pos> result = lookDown(map, lookPos, i);
            pos.insert(pos.end(), result.rbegin(), result.rend());
            pos.push_back(lookPos);
            result = lookUp(map, lookPos, i);
            pos.insert(pos.end(), result.begin(), result.end());
        }
        return pos;
    }

    std::vector<Pos> Player::lookUp(Map* map, Pos startPos, int distance)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= distance; i++){
            lookPos.x = startPos.x;
            lookPos.y = startPos.y - i;
            map->correctPos(&lookPos);
            pos.push_back(lookPos);
        }
        return pos;
    }

    std::vector<Pos> Player::lookDown(Map* map, Pos startPos, int distance)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= distance; i++){
            lookPos.x = startPos.x;
            lookPos.y = startPos.y + i;
            map->correctPos(&lookPos);
            pos.push_back(lookPos);
        }
        return pos;
    }

    std::vector<Pos> Player::lookRight(Map* map, Pos startPos, int distance)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= distance; i++){
            lookPos.x = startPos.x + i;
            lookPos.y = startPos.y;
            map->correctPos(&lookPos);
            pos.push_back(lookPos);
        }
        return pos;
    }

    std::vector<Pos> Player::lookLeft(Map* map, Pos startPos, int distance)
    {
        std::vector<Pos> pos;
        Pos lookPos;

        for (int i = 1; i <= distance; i++){
            lookPos.x = startPos.x - i;
            lookPos.y = startPos.y;
            map->correctPos(&lookPos);
            pos.push_back(lookPos);
        }
        return pos;
    }
}