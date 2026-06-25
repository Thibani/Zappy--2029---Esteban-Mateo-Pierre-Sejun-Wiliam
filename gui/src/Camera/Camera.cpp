#include "Camera.hpp"
#include <cmath>

void PlayerView::init(float mapWidth, float mapHeight) {
    _target = { mapWidth / 2.f, 0.f, mapHeight / 2.f };

    _cam.target     = _target;
    _cam.up         = { 0.f, 1.f, 0.f };
    _cam.fovy       = 45.f;
    _cam.projection = CAMERA_PERSPECTIVE;

    _updatePosition();
}

void PlayerView::_updatePosition() {
    // Clamp vertical angle so camera never flips
    if (_angleV < 10.f)  _angleV = 10.f;
    if (_angleV > 89.f)  _angleV = 89.f;

    float hRad = _angleH * DEG2RAD;
    float vRad = _angleV * DEG2RAD;

    // Spherical to cartesian around _target
    _cam.position = {
        _target.x + _distance * cosf(vRad) * sinf(hRad),
        _target.y + _distance * sinf(vRad),
        _target.z + _distance * cosf(vRad) * cosf(hRad)
    };
    _cam.target = _target;
}

void PlayerView::handleInput(float dt) {
    bool changed = false;

    // --- Orbit (right click + drag OR Q/E to rotate horizontally) ---
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        _angleH -= delta.x * _panSpeed;
        _angleV += delta.y * _panSpeed;
        changed = true;
    }

    if (IsKeyDown(KEY_Q)) { _angleH -= _rotSpeed * dt; changed = true; }
    if (IsKeyDown(KEY_E)) { _angleH += _rotSpeed * dt; changed = true; }

    // --- Tilt (R/F keys) ---
    if (IsKeyDown(KEY_R)) { _angleV += _rotSpeed * dt * 0.5f; changed = true; }
    if (IsKeyDown(KEY_F)) { _angleV -= _rotSpeed * dt * 0.5f; changed = true; }

    // --- Zoom (scroll wheel) ---
    float scroll = GetMouseWheelMove();
    if (scroll != 0.f) {
        _distance -= scroll * _zoomSpeed;
        if (_distance < 3.f)  _distance = 3.f;
        if (_distance > 80.f) _distance = 80.f;
        changed = true;
    }

    // --- Pan target (WASD moves orbit center) ---
    if (IsKeyDown(KEY_W)) { _target.z -= _zoomSpeed * dt * 2.f; changed = true; }
    if (IsKeyDown(KEY_S)) { _target.z += _zoomSpeed * dt * 2.f; changed = true; }
    if (IsKeyDown(KEY_A)) { _target.x -= _zoomSpeed * dt * 2.f; changed = true; }
    if (IsKeyDown(KEY_D)) { _target.x += _zoomSpeed * dt * 2.f; changed = true; }

    if (changed)
        _updatePosition();
}