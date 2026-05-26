#pragma once

#include <iostream>
#include <vector>

#include "SoftwareRenderer/XRenderer.hpp"
#include "SoftwareRenderer/XTexture.hpp"
#include "Game/GameStates.hpp"

class Road {

    float x{}, y{}, w{}, h{};

    Texture* texture{};

    std::vector<XFRect> rects{};

    public:
    Road(float x, float y, float w, float h, Texture* tex);

    void Update();
    void Render(Renderer* renderer);

};