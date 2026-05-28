#pragma once

#include <iostream>
#include <array>

class GameStates {

    public:

    static inline bool game_active{}, game_running{}, game_menu{true}, game_paused{}, game_over{}, game_restart{};

    static inline int score{};
    
    static inline int highscore{};

    static inline float universal_speed{1.5f};
    static inline float max_universal_speed{4.0f};

};