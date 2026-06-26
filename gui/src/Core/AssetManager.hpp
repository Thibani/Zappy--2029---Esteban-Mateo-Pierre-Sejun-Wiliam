#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

class AssetManager {
public:
    explicit AssetManager(const std::string& assetRoot);
    ~AssetManager();

    Texture2D& get(const std::string& name);

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

private:
    std::string                              _root;
    std::unordered_map<std::string, Texture2D> _textures;

    void _loadAll();
};