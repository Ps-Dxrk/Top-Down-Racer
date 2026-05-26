#pragma once

#include <iostream>
#include <vector>
#include "cstdint"
#include "stb_image.h"

class Texture{

    std::vector<uint32_t> pixels{};
    int width{}, height{};

    public:
    Texture(std::string filePath){

        int nrChannels{};

        uint8_t* image_data = stbi_load(filePath.c_str(), &this->width, &this->height, &nrChannels, 4); //loading raw pixel data
        
        if(!image_data){
            std::cerr << "Error Loading STB Image: " << filePath << std::endl;
            return;
        }

        this->pixels.resize(this->width * this->height); //resizing the pixel vector so it can hold all the pixel data;

        for(int i = 0; i < this->width * this->height; ++i){

            int base_index { i * 4 };

            uint8_t red { image_data[base_index + 0] };
            uint8_t green { image_data[base_index + 1] };
            uint8_t blue { image_data[base_index + 2] };
            uint8_t alpha { image_data[base_index + 3] };

            this->pixels[i] = (
                static_cast<uint32_t>(alpha) << 24 |
                static_cast<uint32_t>(red) << 16 |
                static_cast<uint32_t>(green) << 8 |
                static_cast<uint32_t>(blue)
            );

        }

        stbi_image_free(image_data); //freeing the raw pixel data;

        std::cout << "STB Image Successfully Loaded! | FilePath: " << filePath << std::endl;
    }

    uint32_t getPixel(int x, int y){

        if(x >= 0 && y >= 0 && x < this->width && y < this->height){
            return this->pixels[(y * this->width) + x];
        }

        return 0x00000000;

    }

    std::vector<uint32_t>* getPixelData(){ return &this->pixels; }
    int getW() { return this->width; }
    int getH() { return this->height; }

};