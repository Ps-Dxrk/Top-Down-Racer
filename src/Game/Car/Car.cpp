#include "Car.hpp"

float getDyanmicSpeed(int score){
    float base_speed { 1.5f };
    float increment_fac { 0.001f };

    return base_speed + (static_cast<float>(score) * increment_fac);
}

//base car class
Car::Car(float x, float y, float w, float h, Texture* tex) : x(x), y(y), w(w), h(h), texture(tex) {



}
void Car::Update(){

    this->y += GameStates::universal_speed;

}
void Car::Render(Renderer* renderer){

    XRect drawRect(this->x, this->y, this->w, this->h);
    renderer->drawTextureRotated(this->texture, &drawRect, 180.0f);

}
float Car::GetX() const{ return this->x; }
float Car::GetY() const{ return this->y; }
float Car::GetW() const{ return this->w; }
float Car::GetH() const{ return this->h; }

bool Car::getCollided() const{ return this->collided; }
void Car::setCollided(bool val) { this->collided = val; }

bool Car::getScorePassed() const{ return this->scorePassed; }
void Car::setScorePassed(bool val) { this->scorePassed = val; }

//player car class
PlayerCar::PlayerCar(float x, float y, float w, float h, float _speed, Texture* tex) : Car(x, y, w, h, tex), speed(_speed){

}

void PlayerCar::Update() {

    this->speed = getDyanmicSpeed(GameStates::score);

    float dirX{}, dirY{};

    if(Input::GetKeyPressed(XK_d) && this->x + this->w < 355.0f)  dirX += 1.0f;
    if(Input::GetKeyPressed(XK_a) && this->x > 45.0f)             dirX -= 1.0f;
    if(Input::GetKeyPressed(XK_s) && this->y + this->h < 640.0f)  dirY += 1.0f;
    if(Input::GetKeyPressed(XK_w) && this->y > 0.0f)              dirY -= 1.0f;

    if(Input::GetKeyPressed(XK_d) && this->x + this->w < 355.0f && this->steeringAngle < 12.0f) this->steeringAngle += 0.7f;
    if(Input::GetKeyPressed(XK_a) && this->x > 45.0f && this->steeringAngle > -12.0f) this->steeringAngle -= 0.7f;

    if(!Input::GetKeyPressed(XK_d) && this->steeringAngle > 0.0f) this->steeringAngle-=0.9f;
    if(!Input::GetKeyPressed(XK_a) && this->steeringAngle < 0.0f) this->steeringAngle+=0.9f;

    if(dirX != 0.0f && dirY != 0.0f){

        float length {std::sqrtf(dirX * dirX + dirY * dirY)};

        dirX /= length;
        dirY /= length;

    }

    this->x += dirX * this->speed;
    this->y += dirY * this->speed;

}

void PlayerCar::Render(Renderer* renderer){

    XFRect drawRect(this->x, this->y, this->w, this->h);
    renderer->drawTextureRotated(this->texture, &drawRect, this->steeringAngle);

}