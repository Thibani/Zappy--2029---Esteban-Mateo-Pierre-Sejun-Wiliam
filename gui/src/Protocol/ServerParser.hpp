#pragma once
#include <string>
#include "../Map/Map.hpp"
#include "../Character/CharacterFactory.hpp"

class ServerParser {
public:
    void parse(const std::string& line, Map& map, CharacterFactory& factory);
private:
    void _parseMsz(std::istringstream& ss, Map& map);
    void _parseBct(std::istringstream& ss, Map& map);
    void _parsePnw(std::istringstream& ss, CharacterFactory& factory);
    void _parsePpo(std::istringstream& ss, CharacterFactory& factory);
    void _parsePlv(std::istringstream& ss, CharacterFactory& factory);
    void _parsePdi(std::istringstream& ss, CharacterFactory& factory);
};