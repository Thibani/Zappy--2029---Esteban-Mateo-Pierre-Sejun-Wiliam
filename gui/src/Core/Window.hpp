
#pragma once
#include <raylib.h>

struct Window {
    Window(int w, int h, const char* title) {
        InitWindow(w, h, title);
        SetTargetFPS(60);
    }
    ~Window() { CloseWindow(); }
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
};

struct ManagedTexture {
    Texture2D texture;
    explicit ManagedTexture(const char* path) : texture(LoadTexture(path)) {}
    ~ManagedTexture() { UnloadTexture(texture); }
    ManagedTexture(const ManagedTexture&) = delete;
    ManagedTexture& operator=(const ManagedTexture&) = delete;
};