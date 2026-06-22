#include "Player.hpp"
#include "Map.hpp"

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

void Player::moveForward(int mapWidth, int mapHeight)
{
    switch(_direction) {
        case UP:
            _positionY++;
            break;
        case RIGHT:
            _positionX++;
            break;
        case LEFT:
            _positionX--;
            break;
        case DOWN:
            _positionY--;
            break;
    }

    if (_positionX >= mapWidth)
        _positionX = 0;

    if (_positionX < 0)
        _positionX = mapWidth - 1;

    if (_positionY >= mapHeight)
        _positionY = 0;

    if (_positionY < 0)
        _positionY = mapHeight - 1;
}

bool Player::take(Map& map, Ressource ressource)
{
    Case& c = map.getCase(_positionX, _positionY);
    if (c.ressource[ressource] > 0){
        c.ressource[ressource]--;
        _inventory[ressource]++;
        return true;
    }
    return false;
}

bool Player::set(Map& map, Ressource ressource)
{
    Case& c = map.getCase(_positionX, _positionY);
    if (_inventory[ressource] > 0){
        _inventory[ressource]--;
        c.ressource[ressource]++;
        return true;
    }
    return false;
}

bool Player::hungry()
{
    _foodLevel--;
    if (_foodLevel <= 0)
        return true;
    return false;
}

bool Player::eat()
{
    if (_inventory[FOOD] <= 0)
        return false;
    _inventory[FOOD]--;
    _foodLevel++;
    return true;
}