#include "ServerParser.hpp"
#include <sstream>
#include <iostream>

void ServerParser::parse(const std::string& line, Map& map, CharacterFactory& factory, EggFactory& eggs)
{
    if (line.empty())
        return;

    std::istringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if      (cmd == "msz") _parseMsz(ss, map);
    else if (cmd == "bct") _parseBct(ss, map);
    else if (cmd == "pnw") _parsePnw(ss, factory);
    else if (cmd == "ppo") _parsePpo(ss, factory);
    else if (cmd == "plv") _parsePlv(ss, factory);
    else if (cmd == "pdi") _parsePdi(ss, factory);
    else if (cmd == "enw") _parseEnw(ss, eggs);
    else if (cmd == "ebo") _parseEbo(ss, eggs);
    else if (cmd == "edi") _parseEdi(ss, eggs);
    else if (cmd == "WELCOME") return;
    else if (cmd == "sgt")    return;
    else if (cmd == "tna")    return;
    else if (cmd == "suc") std::cerr << "[ServerParser] Unknown command sent by GUI\n";
    else if (cmd == "sbp") std::cerr << "[ServerParser] Bad parameters sent by GUI\n";
    else std::cerr << "[ServerParser] Unhandled command: " << cmd << "\n";
}

// msz X Y
void ServerParser::_parseMsz(std::istringstream& ss, Map& map)
{
    int x, y;
    ss >> x >> y;
    map.resize(x, y);
    std::cout << "[ServerParser] Map resized to " << x << "x" << y << "\n";
}

// bct X Y q0 q1 q2 q3 q4 q5 q6
void ServerParser::_parseBct(std::istringstream& ss, Map& map)
{
    int x, y;
    ss >> x >> y;
    Tile& tile = map.getTile(x, y);
    ss >> tile.q0 >> tile.q1 >> tile.q2 >> tile.q3
       >> tile.q4 >> tile.q5 >> tile.q6;
}

// pnw #n X Y O L NAME
void ServerParser::_parsePnw(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr, name;
    int x, y, o, l;
    ss >> idStr >> x >> y >> o >> l >> name;

    // Strip leading '#'
    int id = std::stoi(idStr.substr(1));
    factory.createCharacter(id, name, x, y, l);
    std::cout << "[ServerParser] Player #" << id << " (" << name << ") spawned at " << x << "," << y << "\n";
}

// ppo #n X Y O
void ServerParser::_parsePpo(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr;
    int x, y, o;
    ss >> idStr >> x >> y >> o;

    int id = std::stoi(idStr.substr(1));
    Character* c = factory.findById(id);
    if (c) {
        c->tileX = x;
        c->tileY = y;
    }
}

// plv #n L
void ServerParser::_parsePlv(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr;
    int level;
    ss >> idStr >> level;

    int id = std::stoi(idStr.substr(1));
    Character* c = factory.findById(id);
    if (c)
        c->level = level;
}

// pdi #n
void ServerParser::_parsePdi(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr;
    ss >> idStr;

    int id = std::stoi(idStr.substr(1));
    factory.removeById(id);
    std::cout << "[ServerParser] Player #" << id << " died\n";
}

// enw #e #n X Y
void ServerParser::_parseEnw(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr, parentIdStr;
    int x, y;
    ss >> eggIdStr >> parentIdStr >> x >> y;
    int eggId    = std::stoi(eggIdStr.substr(1));
    int parentId = std::stoi(parentIdStr.substr(1));
    eggs.addEgg(eggId, parentId, x, y);
}

// ebo #e
void ServerParser::_parseEbo(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr;
    ss >> eggIdStr;
    eggs.removeById(std::stoi(eggIdStr.substr(1)));
}

// edi #e
void ServerParser::_parseEdi(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr;
    ss >> eggIdStr;
    eggs.removeById(std::stoi(eggIdStr.substr(1)));
}