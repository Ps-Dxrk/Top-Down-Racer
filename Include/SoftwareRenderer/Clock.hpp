#pragma once

#include <chrono>

class Clock{

    static inline std::chrono::time_point<std::chrono::steady_clock> start_time{};

    public:
    static inline void Initialize(){

        start_time = std::chrono::steady_clock::now();

    }

    static inline uint64_t GetTimeNow(){ //returns time elapsed since initalizing this clock in milliseconds

        std::chrono::time_point<std::chrono::steady_clock> timeNow { std::chrono::steady_clock::now() };

        std::chrono::milliseconds elapsed { std::chrono::duration_cast<std::chrono::milliseconds>(timeNow - start_time) };

        return static_cast<uint64_t>(elapsed.count());

    }
};