#pragma once

#include <iostream>
#include <memory>
#include <cmath>
#include <algorithm>

#include "SoftwareRenderer/Input.hpp"
#include "SoftwareRenderer/XRect.hpp"
#include "SoftwareRenderer/XTexture.hpp"
#include "SoftwareRenderer/XRenderer.hpp"

#include "Game/Assets/AssetLoad.hpp"
#include "Game/GameStates.hpp"

class Button{

    float x{}, y{}, w{}, h{};

    Texture* texture{};

    bool clicked{};

    float hoverAniT{};
    float inX{}, inY{}, inW{}, inH{};

    public:
    Button(float x, float y, float w, float h, Texture* tex);

    bool checkClicked();

    void hoverAni();

    void Update();
    void Render(Renderer* renderer);

};

class UI {

    //title text texture
    Texture* MainTitleTextTex{};


    //UI buttons
    std::unique_ptr<Button> PlayButton{};
    std::unique_ptr<Button> ExitButton{};

    public:
    UI();

    void Update();
    void Render(Renderer* renderer);
};