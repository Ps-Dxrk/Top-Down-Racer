#include "UI.hpp"

bool leftMouseclicked{};

bool MouseButtonCollision(float x, float y, const XFRect* rect){

    return x >= rect->x && y >= rect->y && x <= rect->x + rect->w && y <= rect->y + rect->h;

}

UI::UI(){

    //main title
    this->MainTitleTextTex = Assets::getTexture(UI_MAIN_TITLE_TEX);

    //UI buttons
    this->PlayButton = std::make_unique<Button>(100.0f, 430.0f, 55.0f, 55.0f, Assets::getTexture(UI_PLAY_BUTTON));
    this->ExitButton = std::make_unique<Button>(240.0f, 430.0f, 55.0f, 55.0f, Assets::getTexture(UI_EXIT_BUTTON));

}

void UI::Update(){

   if(GameStates::game_menu){
        // Play button update
        if(this->PlayButton->checkClicked()) GameStates::game_menu = false;
        this->PlayButton->Update();

        //exit button update
        if(this->ExitButton->checkClicked()) GameStates::game_running = false;
        this->ExitButton->Update();
   }

    if(Input::GetMouseButton(Button1) && !leftMouseclicked) leftMouseclicked = true;
    else if(!Input::GetMouseButton(Button1)) leftMouseclicked = false;

}

void UI::Render(Renderer* renderer){

    //menu rendering
    if(GameStates::game_menu && GameStates::game_running){

        //menu title text
        XFRect menuTitleTexDrawRect{75.0f, 120.0f, 250.0f, 100.0f};
        renderer->drawTexture(this->MainTitleTextTex, &menuTitleTexDrawRect);

        //rendering buttons
        this->PlayButton->Render(renderer);
        this->ExitButton->Render(renderer);

    }
}

/************************************************************************************ */

//buton definition
Button::Button(float x, float y, float w, float h, Texture* tex) : x(x), y(y), w(w), h(h), texture(tex), inX(x), inY(y), inW(w), inH(h){

}

bool Button::checkClicked(){

    XFRect collisionRect{this->x + 3.0f, this->y + 3.0f, this->w - 6.0f, this->h - 6.0f};

    if(Input::GetMouseButton(Button1) && !leftMouseclicked && !this->clicked && MouseButtonCollision(Input::GetMouseX(), Input::GetMouseY(), &collisionRect)){
        this->clicked = true;
        leftMouseclicked = true;
        return true;
    }

    if(!Input::GetMouseButton(Button1) && leftMouseclicked && this->clicked){
        this->clicked = false;
        leftMouseclicked = false;
        return false;
    }

    return false;

}

void Button::hoverAni(){

    XFRect collisionRect{this->x + 2.0f, this->y + 2.0f, this->w - 4.0f, this->h - 4.0f};

    if(MouseButtonCollision(Input::GetMouseX(), Input::GetMouseY(), &collisionRect)) this->hoverAniT += 0.1f;
    else this->hoverAniT -= 0.1;

    this->hoverAniT = std::clamp(this->hoverAniT, 0.0f, 1.0f);

    //changing the X, Y , W and H
    this->x = std::lerp(inX, inX - 3.0f, this->hoverAniT);
    this->y = std::lerp(inY, inY - 3.0f, this->hoverAniT);
    this->w = std::lerp(inW, inW + 6.0f, this->hoverAniT);
    this->h = std::lerp(inH, inH + 6.0f, this->hoverAniT);
}


void Button::Update(){

    this->hoverAni();

}

void Button::Render(Renderer* renderer){

    XFRect drawRect{this->x, this->y, this->h, this->w};
    renderer->drawTexture(this->texture, &drawRect);

}