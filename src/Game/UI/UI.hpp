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
#include "Game/Settings.hpp"

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

    //game over text
    Texture* GameOverTextTex{};


    //UI buttons
    std::unique_ptr<Button> PlayButtonMenu{};
    std::unique_ptr<Button> ExitButtonMenu{};
    std::unique_ptr<Button> RetryButtonGameOver{};
    std::unique_ptr<Button> ExitButtonGameOver{};

    public:
    UI();

    void Update();
    void Render(Renderer* renderer);
};