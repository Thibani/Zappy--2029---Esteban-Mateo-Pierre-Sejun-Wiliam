#pragma once
#include <string>
#include <sstream>
#include "../Map/Map.hpp"
#include "../Character/CharacterFactory.hpp"
#include "../Egg/EggFactory.hpp"

class ServerParser {
public:
    void parse(const std::string& line, Map& map, CharacterFactory& factory, EggFactory& eggs);
private:
    // Map
    void _parseMsz(std::istringstream& ss, Map& map);
    void _parseBct(std::istringstream& ss, Map& map);
    // Players
    void _parsePnw(std::istringstream& ss, CharacterFactory& factory);
    void _parsePpo(std::istringstream& ss, CharacterFactory& factory);
    void _parsePlv(std::istringstream& ss, CharacterFactory& factory);
    void _parsePdi(std::istringstream& ss, CharacterFactory& factory);
    void _parsePin(std::istringstream& ss, CharacterFactory& factory);
    void _parsePgt(std::istringstream& ss);
    void _parsePdr(std::istringstream& ss);
    void _parsePex(std::istringstream& ss);
    void _parsePbc(std::istringstream& ss);
    void _parsePfk(std::istringstream& ss);
    // Incantation
    void _parsePic(std::istringstream& ss);
    void _parsePie(std::istringstream& ss);
    // Eggs
    void _parseEnw(std::istringstream& ss, EggFactory& eggs);
    void _parseEbo(std::istringstream& ss, EggFactory& eggs);
    void _parseEdi(std::istringstream& ss, EggFactory& eggs);
    // Server
    void _parseSgt(std::istringstream& ss);
    void _parseSst(std::istringstream& ss);
    void _parseSeg(std::istringstream& ss);
    void _parseSmg(std::istringstream& ss);
};