#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <unistd.h>
#include <thread>

#include "SoftwareRenderer/Clock.hpp"
#include "SoftwareRenderer/Window.hpp"
#include "SoftwareRenderer/XRenderer.hpp"

#include "Game/GameStates.hpp"
#include "Game/Game.hpp"

int main(){

    if(!wWindow::LibInit()){
        std::cerr << "Couldn't Establish Connection with X Display Server!\n";
        return -1;
    }

    Clock::Initialize(); //initalizing the clock

    wWindow window("X11 Software Renderer", 400, 640, nullptr);
    Renderer renderer(&window);

    //game stuff
    Game game;

    //update and render deltas
    int FPS {100};
    int UPS{150};
    float frameTime{1000.0f/FPS}, updateTime{1000.0f/UPS};
    float deltaU{}, deltaR{};
    uint64_t cur_time{};
    uint64_t last_time { Clock::GetTimeNow() };

    uint64_t cur_fps {0};
    uint64_t cur_ups {0};
    float fps_ups_timer {0};

    GameStates::game_running = true;
    while(GameStates::game_running){

        //time updates(deltaupdate, deltarender, fps_timer and stuff)
        cur_time = Clock::GetTimeNow();
        deltaU += static_cast<float>(cur_time - last_time);
        deltaR += static_cast<float>(cur_time - last_time);
        fps_ups_timer += static_cast<float>(cur_time - last_time);
        last_time = cur_time;

        //event polling
        window.EventPolling();
        if(window.checkWindowClose()) GameStates::game_running = false;


        //update delta timer
        while(deltaU >= updateTime){    

            //update
            game.Update();

            cur_ups++;
            deltaU -= updateTime;
        }

        //render delta timer
        while(deltaR >= frameTime){

            renderer.ClearBuffer(); //clearing the pixel buffer
        
            //rendering code
            game.Render(&renderer);

            renderer.SwapRenderBuffers();

            cur_fps++;
            deltaR -= frameTime;
        }

        //fps display and timer
        if(fps_ups_timer >= 1000.0f){
            printf("FPS: %ld | UPS: %ld\r", cur_fps, cur_ups);
            fflush(stdout);

            cur_fps = 0;
            cur_ups = 0;
            fps_ups_timer = 0.0f;
        }
    }

    std::cout << "X Display Closed successfully!\n";

    renderer.Destroy();
    window.Destroy();

    return 0;
}