#include "ServerParser.hpp"
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
    else if (cmd == "pin") _parsePin(ss, factory);
    else if (cmd == "pgt") _parsePgt(ss);
    else if (cmd == "pdr") _parsePdr(ss);
    else if (cmd == "pex") _parsePex(ss);
    else if (cmd == "pbc") _parsePbc(ss);
    else if (cmd == "pfk") _parsePfk(ss);
    else if (cmd == "pic") _parsePic(ss);
    else if (cmd == "pie") _parsePie(ss);
    else if (cmd == "enw") _parseEnw(ss, eggs);
    else if (cmd == "ebo") _parseEbo(ss, eggs);
    else if (cmd == "edi") _parseEdi(ss, eggs);
    else if (cmd == "sgt") _parseSgt(ss);
    else if (cmd == "sst") _parseSst(ss);
    else if (cmd == "seg") _parseSeg(ss);
    else if (cmd == "smg") _parseSmg(ss);
    else if (cmd == "tna") return; // team names — not needed for rendering
    else if (cmd == "WELCOME") return;
    else if (cmd == "suc") std::cerr << "[ServerParser] Unknown command sent by GUI\n";
    else if (cmd == "sbp") std::cerr << "[ServerParser] Bad parameters sent by GUI\n";
    else std::cerr << "[ServerParser] Unhandled: " << cmd << "\n";
}

// --- Map ---

void ServerParser::_parseMsz(std::istringstream& ss, Map& map)
{
    int x, y;
    ss >> x >> y;
    map.resize(x, y);
    std::cout << "[ServerParser] Map resized to " << x << "x" << y << "\n";
}

void ServerParser::_parseBct(std::istringstream& ss, Map& map)
{
    int x, y;
    ss >> x >> y;
    Tile& tile = map.getTile(x, y);
    ss >> tile.q0 >> tile.q1 >> tile.q2 >> tile.q3
       >> tile.q4 >> tile.q5 >> tile.q6;
}

// --- Players ---

void ServerParser::_parsePnw(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr, team;
    int x, y, o, l;
    ss >> idStr >> x >> y >> o >> l >> team;
    int id = std::stoi(idStr.substr(1));
    factory.createCharacter(id, "Player#" + std::to_string(id), team, x, y, l);
}

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

void ServerParser::_parsePdi(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr;
    ss >> idStr;
    int id = std::stoi(idStr.substr(1));
    factory.removeById(id);
    std::cout << "[ServerParser] Player #" << id << " died\n";
}

// pin #n X Y q0..q6 — update player inventory (stored on Character for future HUD use)
void ServerParser::_parsePin(std::istringstream& ss, CharacterFactory& factory)
{
    std::string idStr;
    int x, y, q0, q1, q2, q3, q4, q5, q6;
    ss >> idStr >> x >> y >> q0 >> q1 >> q2 >> q3 >> q4 >> q5 >> q6;
    int id = std::stoi(idStr.substr(1));
    Character* c = factory.findById(id);
    if (c) {
        c->inv[0] = q0; c->inv[1] = q1; c->inv[2] = q2;
        c->inv[3] = q3; c->inv[4] = q4; c->inv[5] = q5;
        c->inv[6] = q6;
    }
}

// pgt #n i — player picked up resource i, server follows with bct + pin
void ServerParser::_parsePgt(std::istringstream& ss)
{
    std::string idStr;
    int i;
    ss >> idStr >> i;
    std::cout << "[ServerParser] Player " << idStr << " picked up resource " << i << "\n";
}

// pdr #n i — player dropped resource i, server follows with bct + pin
void ServerParser::_parsePdr(std::istringstream& ss)
{
    std::string idStr;
    int i;
    ss >> idStr >> i;
    std::cout << "[ServerParser] Player " << idStr << " dropped resource " << i << "\n";
}

// pex #n — player was ejected, server follows with ppo
void ServerParser::_parsePex(std::istringstream& ss)
{
    std::string idStr;
    ss >> idStr;
    std::cout << "[ServerParser] Player " << idStr << " ejected\n";
}

// pbc #n MSG — broadcast, message may contain spaces
void ServerParser::_parsePbc(std::istringstream& ss)
{
    std::string idStr, msg;
    ss >> idStr;
    std::getline(ss >> std::ws, msg);
    std::cout << "[ServerParser] Broadcast from " << idStr << ": " << msg << "\n";
}

// pfk #n — player forked, server follows with enw
void ServerParser::_parsePfk(std::istringstream& ss)
{
    std::string idStr;
    ss >> idStr;
    std::cout << "[ServerParser] Player " << idStr << " forked\n";
}

// --- Incantation ---

// pic X Y L #n1 #n2 ... — incantation started
void ServerParser::_parsePic(std::istringstream& ss)
{
    int x, y, l;
    ss >> x >> y >> l;
    std::cout << "[ServerParser] Incantation started at " << x << "," << y << " level " << l << "\n";
}

// pie X Y R — incantation ended (R=1 success, R=0 fail)
void ServerParser::_parsePie(std::istringstream& ss)
{
    int x, y, r;
    ss >> x >> y >> r;
    std::cout << "[ServerParser] Incantation " << (r ? "succeeded" : "failed")
              << " at " << x << "," << y << "\n";
}

// --- Eggs ---

void ServerParser::_parseEnw(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr, parentIdStr;
    int x, y;
    ss >> eggIdStr >> parentIdStr >> x >> y;
    int eggId    = std::stoi(eggIdStr.substr(1));
    int parentId = std::stoi(parentIdStr.substr(1));
    eggs.addEgg(eggId, parentId, x, y);
}

void ServerParser::_parseEbo(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr;
    ss >> eggIdStr;
    eggs.removeById(std::stoi(eggIdStr.substr(1)));
}

void ServerParser::_parseEdi(std::istringstream& ss, EggFactory& eggs)
{
    std::string eggIdStr;
    ss >> eggIdStr;
    eggs.removeById(std::stoi(eggIdStr.substr(1)));
}

// --- Server ---

void ServerParser::_parseSgt(std::istringstream& ss)
{
    int t;
    ss >> t;
    std::cout << "[ServerParser] Time unit: " << t << "\n";
}

void ServerParser::_parseSst(std::istringstream& ss)
{
    int t;
    ss >> t;
    std::cout << "[ServerParser] Time unit changed to: " << t << "\n";
}

void ServerParser::_parseSeg(std::istringstream& ss)
{
    std::string team;
    ss >> team;
    std::cout << "[ServerParser] Game over — winner: " << team << "\n";
}

void ServerParser::_parseSmg(std::istringstream& ss)
{
    std::string msg;
    std::getline(ss >> std::ws, msg);
    std::cout << "[ServerParser] Server message: " << msg << "\n";
}