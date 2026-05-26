#pragma once

#include <iostream>
#include<map>
#include <array>

#include "X11/Xlib.h"
#include "X11/Xutil.h"

class Input {

    //creates a map to store all the keys, keySym is the key and bool is the value that the key holds, can be either true or false
    static inline std::map<KeySym, bool> keys{};
    static inline std::array<bool, 6> mouseKeys{ false };

    static inline float mouseX{};
    static inline float mouseY{};

    public:

    static inline void SetKey(KeySym keysym, bool val){
        Input::keys[keysym] = val;
    }

    static inline bool GetKeyPressed(KeySym keysym){
        
        if(Input::keys.contains(keysym)){
            return Input::keys[keysym];
        }
        
        return false;
    }   

    static inline void SetMouseButton(int Button, bool val){
        if(Button >= 1 && Button <= 5) Input::mouseKeys.at(Button) = val;
    }
    static inline bool GetMouseButton(int Button){
        if(Button >= 1 && Button <= 5) return Input::mouseKeys.at(Button);

        return false;
    }

    static inline void SetMousePos(float x, float y){
        Input::mouseX = x; Input::mouseY = y;
    }

    static inline float GetMouseX(){ return Input::mouseX; }
    static inline float GetMouseY() { return Input::mouseY; }
};