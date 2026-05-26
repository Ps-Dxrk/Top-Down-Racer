#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <array>

#include "SoftwareRenderer/XTexture.hpp"

enum tex_index{

    CAR_TEX_AUDI,

    TEX_DESERT_ROAD,

    CAR_TEX_NORMAL,

    CAR_TEX_MINI_TRUCK,

    CAR_TEX_TAXI,

    UI_MAIN_TITLE_TEX,

    UI_PLAY_BUTTON,

    UI_PAUSE_BUTTON,

    UI_EXIT_BUTTON

};


class Assets {

    static inline std::vector<Texture> textures{};

    public:

    static void Init();
    static Texture* getTexture(int _i);

};