#pragma once

#include <iostream>

#include "SoftwareRenderer/Input.hpp"
#include "SoftwareRenderer/XRenderer.hpp"
#include "SoftwareRenderer/XRect.hpp"

#include "Game/Assets/AssetLoad.hpp"
#include "Game/GameStates.hpp"

class Car {

    protected:
    float x{}, y{}, w{}, h{};

    Texture* texture{};

    bool collided{};

    bool scorePassed{};

    public:
    Car(float x, float y, float w, float h, Texture* tex);

    virtual ~Car() = default; //critical if i delete a Car* pointing to a different car like playercar that inherits Car

    virtual void Update();
    virtual void Render(Renderer* renderer);

    //getters and setters
    float GetX() const;
    float GetY() const;
    float GetW() const;
    float GetH() const;

    bool getCollided() const;
    void setCollided(bool val);

    bool getScorePassed() const;
    void setScorePassed(bool val);

};

class PlayerCar : public Car {

    float speed{};
    float steeringAngle{};

    public:
    PlayerCar(float x, float y, float w, float h, float _speed, Texture* tex);

    void Update() override;
    void Render(Renderer* renderer) override;

};