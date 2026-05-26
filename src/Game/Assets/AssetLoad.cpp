#include "AssetLoad.hpp"

void Assets::Init(){

    Assets::textures.emplace_back("res//cars//Audi.png"); //tex_index = 0(CAR_TEX_AUDI);
    Assets::textures.emplace_back("res//Levels//Desert_Road.png"); //tex_index = 1(TEX_DESERT_ROAD);
    Assets::textures.emplace_back("res//cars//Car.png"); //tex_index = 2(CAR_TEX_NORMAL);
    Assets::textures.emplace_back("res//cars//Mini_truck.png"); //tex_index = 3(CAR_TEX_MINI_TRUCK)
    Assets::textures.emplace_back("res//cars//taxi.png"); //tex_index = 4(CAR_TEX_TAXI);
    Assets::textures.emplace_back("res//UI/Main_Title.png"); //tex_index = 5(UI_MAIN_TITLE_TEX);
    Assets::textures.emplace_back("res//UI//Buttons//PlayButton.png"); //tex_index = 6(UI_PLAY_BUTTON);
    Assets::textures.emplace_back("res//UI//Buttons//PauseButton.png"); //tex_index = 7(UI_PAUSE_BUTTON);
    Assets::textures.emplace_back("res//UI//Buttons//ExitButton.png"); //tex_index = 8(UI_EXIT_BUTTON);

}

Texture* Assets::getTexture(int _i){

    if(_i < 0 || _i >= Assets::textures.size()){
        std::cout << "ERROR: SIZE OF TEXTURE ARRAY: " << Assets::textures.size() << std::endl;
        return nullptr;
    }

    return &Assets::textures.at(_i);

}