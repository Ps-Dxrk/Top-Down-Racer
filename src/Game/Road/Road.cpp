#include "Road.hpp"

Road::Road(float x, float y, float w, float h, Texture* tex) : x(x), y(y), w(w), h(h), texture(tex){

    for(int i = 0; i < 4; i++){
        this->rects.emplace_back(this->x, static_cast<float>(i) * this->h, this->w, this->h);
    }

}

void Road::Update(){

    for(auto& r : this->rects){
        r.y += GameStates::universal_speed;

        if(r.y > 640.0f){
            r.y = r.y - 640.0f - r.h;
        }
    }
}

void Road::Render(Renderer* renderer){

    for(auto& r : this->rects){

        renderer->drawTexture(this->texture, &r);

    }

}