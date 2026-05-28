#pragma once

#include <iostream>

class Settings{

    static inline int window_width{}, window_height{};

    public:
    static inline void setWindowParams(int _w, int _h){
        window_height = _h;
        window_width = _w;
    }

    static inline int getWindowHeight(){ return window_height; }
    static inline int getWindowWidth(){ return window_width; }

};