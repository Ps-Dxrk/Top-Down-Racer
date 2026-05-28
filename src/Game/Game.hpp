#pragma once

#include <iostream>
#include <memory>
#include <random>
#include <array>

#include "SoftwareRenderer/XRenderer.hpp"
#include "SoftwareRenderer/Input.hpp"

#include "Car/Car.hpp"
#include "Assets/AssetLoad.hpp"

#include "Settings.hpp"
#include "Road/Road.hpp"
#include "GameStates.hpp"
#include "UI/UI.hpp"

class Game{

    //cars  
    
    //car random spawn shit
    int car_spawn_timer{}, car_spawn_trigger{}, min_car_spawn_trigger{};

    //car objects
    std::vector<std::unique_ptr<Car>> base_cars{};
    std::unique_ptr<Car> playerCar{};

    //road
    std::unique_ptr<Road> road{};

    //UI
    std::unique_ptr<UI> ui{};

    public:
    Game();

    //cars spawning
    void SpawnCars();
    void RemoveCars();

    //cars collision
    void PlayerCarCollision();

    //score logic
    void PlayerCarScorePass();

    //resetting game
    void Reset();

    void Update();
    void Render(Renderer* renderer);

    private:
    bool collisionCheck(const XFRect* rect_1, const XFRect* rect_2){
        return rect_1->x + rect_1->w >= rect_2->x && rect_1->x <= rect_2->x + rect_2->w && rect_1->y + rect_1->h >= rect_2->y && rect_1->y <= rect_2->y + rect_2->h;
    }

};