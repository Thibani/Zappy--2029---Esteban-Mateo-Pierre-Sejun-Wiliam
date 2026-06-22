#include "GameLogic/Map.hpp"
#include <iostream>

int main()
{
    Map map(5, 5);
    map.setRessource();

    Player player;

    // Test direction initiale
    std::cout << "=== Test directions ===" << std::endl;
    std::cout << "Direction initiale: " << player.getDirection() << std::endl;

    player.turnRight();
    std::cout << "Apres turnRight: " << player.getDirection() << std::endl;

    player.turnRight();
    std::cout << "Apres turnRight: " << player.getDirection() << std::endl;

    player.turnLeft();
    std::cout << "Apres turnLeft: " << player.getDirection() << std::endl;

    // Test deplacement
    std::cout << "\n=== Test deplacement ===" << std::endl;
    std::cout << "Position initiale: (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;

    player.moveForward(map.getWidth(), map.getHeight());
    std::cout << "Apres moveForward: (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;

    // Test wrap-around
    std::cout << "\n=== Test wrap-around ===" << std::endl;
    for (int i = 0; i < 6; i++)
        player.moveForward(map.getWidth(), map.getHeight());
    std::cout << "Apres 6 moveForward (wrap): (" << player.getPositionX() << ", " << player.getPositionY() << ")" << std::endl;

    return 0;
}