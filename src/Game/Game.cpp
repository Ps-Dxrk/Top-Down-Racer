#include "Game.hpp"

float getDynamicIncrement(float base_val, float increment_fac, int score){

    return base_val + (static_cast<float>(score) * increment_fac);

}

Game::Game(){

    Assets::Init(); //loading all assets into memory!

    //car spawn shit
    this->min_car_spawn_trigger = 65;
    this->car_spawn_trigger = 150;

    //cars

    //player car
    this->playerCar = std::make_unique<PlayerCar>(177.0f, Settings::getWindowHeight() - 100.0f, 40.0f, 80.0f, 1.5f, Assets::getTexture(CAR_TEX_AUDI));

    //road 
    this->road = std::make_unique<Road>(0.0f, 0.0f, static_cast<float>(Settings::getWindowWidth()), 220.0f, Assets::getTexture(TEX_DESERT_ROAD));

    //UI
    this->ui = std::make_unique<UI>();
}

void Game::SpawnCars(){

    this->car_spawn_timer++;

    if(this->car_spawn_timer >= this->car_spawn_trigger){
        std::random_device rd;
        std::mt19937 generator{rd()};

        std::uniform_int_distribution<int> carSpawnXRange{58, 303};
        std::uniform_int_distribution<int> carSpawnYRange{150, 250};

        std::array<float, 4> carWidth{40.0f, 50.0f, 40.0f, 38.0f};
        std::array<float, 4> carHeight{80.0f, 100.0f, 75.0f, 72.0f};
        std::array<int, 4> carTexIndices{
            CAR_TEX_AUDI, CAR_TEX_MINI_TRUCK, CAR_TEX_NORMAL, CAR_TEX_TAXI
        };
        std::uniform_int_distribution<int> carType{0, carWidth.size() - 1};
        int carTypeIndex { carType(generator) };

        XFRect carRect{
            static_cast<float>(carSpawnXRange(generator)), //xpos
            -(static_cast<float>(carSpawnYRange(generator))), //ypos
            carWidth.at(carTypeIndex), //width
            carHeight.at(carTypeIndex), // height
        };

        bool carRectCollided{false};
        bool carRectInLine{false};

        do{

            carRectCollided = false;
            carRectInLine = false;

            if(carRect.x + carRect.w >= 195.0f && carRect.x <= 205.0f) carRectInLine = true;

            for(auto& r : this->base_cars){
                XFRect curCarRect{r->GetX(), r->GetY(), r->GetW(), r->GetH()};
                if(this->collisionCheck(&carRect, &curCarRect)) carRectCollided = true;
            }

            if(carRectCollided || carRectInLine){
                carRect = {
                    static_cast<float>(carSpawnXRange(generator)), //xpos
                    -(static_cast<float>(carSpawnYRange(generator))), //ypos
                    carWidth.at(carTypeIndex), //width
                    carHeight.at(carTypeIndex), // height
                };
            }

        } while(carRectCollided || carRectInLine);

        this->base_cars.push_back(std::make_unique<Car>(carRect.x, carRect.y, carRect.w, carRect.h, Assets::getTexture(carTexIndices.at(carTypeIndex))));

        this->car_spawn_timer = 0;
    }

}

void Game::RemoveCars(){

    for(size_t i = 0; i < this->base_cars.size(); ++i){
        if(this->base_cars.at(i)->GetY() > 700.0f) this->base_cars.erase(this->base_cars.begin() + i);
    }

}

void Game::PlayerCarCollision(){

    //checking player car collision with other base cars
    XFRect PlayerCarColRect{this->playerCar->GetX() + 5.0f, this->playerCar->GetY() + 5.0f, this->playerCar->GetW() - 10.0f, this->playerCar->GetH() - 10.0f};

    for(auto& c : this->base_cars){
        XFRect BaseCarColRect{c->GetX() + 5.0f, c->GetY() + 5.0f, c->GetW() - 10.0f, c->GetH() - 10.0f};
        if(this->collisionCheck(&PlayerCarColRect, &BaseCarColRect) && !c->getCollided()){
            c->setCollided(true);
            GameStates::game_over = true;
        }
    }

    //changing the universal speed according to score
    GameStates::universal_speed = getDynamicIncrement(1.5f, 0.01f, GameStates::score);
    if(GameStates::universal_speed >= GameStates::max_universal_speed) GameStates::universal_speed = GameStates::max_universal_speed;

    this->car_spawn_trigger = static_cast<int>(getDynamicIncrement(150.0f, -0.35f, GameStates::score));
    if(this->car_spawn_trigger < this->min_car_spawn_trigger) this->car_spawn_trigger = this->min_car_spawn_trigger;

    //std::cout << "SCORE: " << GameStates::score << " | TRIGGER: " << this->car_spawn_trigger << "\n";
}

void Game::PlayerCarScorePass(){

    //when the bottom part of player car passes the bottom part of the incoming car, score updates(updates 1 every time)
    for(auto& c : this->base_cars){
        if(this->playerCar->GetY() + this->playerCar->GetH() < c->GetY() && !c->getScorePassed()){
            c->setScorePassed(true);
            GameStates::score++;
        }
    }

    //std::cout << "Score: " << GameStates::score << '\n';

}

void Game::Reset(){

    this->base_cars.clear();

    //resetting the player car
    this->playerCar.reset();
    this->playerCar = std::make_unique<PlayerCar>(177.0f, Settings::getWindowHeight() - 100.0f, 40.0f, 80.0f, 1.5f, Assets::getTexture(CAR_TEX_AUDI));

    GameStates::game_restart = false;

}

void Game::Update(){

    //road shit
    if(!GameStates::game_over) this->road->Update();

   if(!GameStates::game_menu && !GameStates::game_over && !GameStates::game_paused){
        //car shit
        this->SpawnCars();
        this->RemoveCars();

        //playercar collision
        this->PlayerCarCollision();

        //playercar score
        this->PlayerCarScorePass();

        for(auto& cars : this->base_cars) cars->Update();
        this->playerCar->Update();
   }

    //UI update
    this->ui->Update();

    if(GameStates::game_restart) this->Reset();
}

void Game::Render(Renderer* renderer){

    //road shit
    this->road->Render(renderer);

    //car shit
    for(auto& cars : this->base_cars) cars->Render(renderer);
    this->playerCar->Render(renderer);

    //ui render
    this->ui->Render(renderer);
}