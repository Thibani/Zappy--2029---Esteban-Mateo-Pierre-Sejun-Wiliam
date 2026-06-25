#include "AssetManager.hpp"
#include <iostream>
#include <vector>

AssetManager::AssetManager(const std::string& assetRoot) : _root(assetRoot)
{
    _loadAll();
}

AssetManager::~AssetManager()
{
    for (auto& [name, tex] : _textures)
        UnloadTexture(tex);
}

void AssetManager::_loadAll()
{
    std::vector<std::pair<std::string, std::string>> entries = {
        { "character_l1", "Characters/Peasant_lv1.png" },
        { "character_l2", "Characters/Looter_lv2.png" },
        { "character_l3", "Characters/Militiaman_lv3.png" },
        { "character_l4", "Characters/Footman_lv4.png" },
        { "character_l5", "Characters/Soldier_lv5.png" },
        { "character_l6", "Characters/Knight_lv6.png" },
        { "character_l7", "Characters/Commander_lv7.png" },
        { "character_l8", "Characters/Lord_lv8.png" },
        { "egg",          "Characters/HeiseneggBaby.png" },
        { "dragonscale",  "Ressources/Dragonscale.png" },
        { "Fang",        "Ressources/Fang.png" },
        { "GoldOre",     "Ressources/GoldOre.png" },
        { "IronOre",     "Ressources/IronOre.png" },
        { "Ruby",        "Ressources/Ruby.png" },
        { "Starshard",   "Ressources/Starshard.png" },
        { "Stone",       "Ressources/Stone.png"     },
        { "Wood",        "Ressources/Wood.png"      },
        { "food",        "Ressources/Food.png"      },
    };

    for (auto& [name, file] : entries) {
        std::string path = _root + "/" + file;
        Texture2D tex = LoadTexture(path.c_str());
        if (tex.id == 0)
            std::cerr << "[AssetManager] Failed to load: " << path << "\n";
        else
            _textures[name] = tex;
    }
}

Texture2D& AssetManager::get(const std::string& name)
{
    auto it = _textures.find(name);
    if (it == _textures.end()) {
        std::cerr << "[AssetManager] Texture not found: " << name << "\n";
        return _textures.begin()->second; // fallback to first texture
    }
    return it->second;
}