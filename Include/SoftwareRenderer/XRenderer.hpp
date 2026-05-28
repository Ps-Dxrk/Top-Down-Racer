#pragma once

#include <iostream>
#include <cmath>
#include <cstdint>
#include "Window.hpp"
#include <vector>
#include "XRect.hpp"
#include "XTexture.hpp"

class Renderer {

    wWindow* win{};

    GC gc{}; //graphics context
    
    std::vector<uint32_t> pixelBuffer{}; //raw pixel buffer on memory

    Visual* visual{}; //tells X_Image how to use the color values according to the current display
    XImage* x_image{}; //the image that we write the color values into;

    public:
    Renderer(wWindow* win) : win(win){

        gc = XCreateGC(this->win->getDisplay(), *this->win->getRawWindow(), 0, nullptr); //creating graphics context;

        this->pixelBuffer.resize(this->win->getWidth() * this->win->getHeight()); //resizing the pixel buffer to fit the window
        
        for(int i = 0; i < this->win->getWidth() * this->win->getHeight(); ++i){
            this->pixelBuffer[i] = this->colorCode(0, 0, 0, 255); //setting all pixels to black color;
        }

        //visual makes sure that XImage uses the correct color format to map the pixels onto the screen, display stucture has the detials of the current display which is being used by the visual
        this->visual = DefaultVisual(this->win->getDisplay(), this->win->getScreen());
        //this is the main image or context that we write the pixel data into, when we call XPutImage() this image will be mapped to the screen
        this->x_image = XCreateImage(this->win->getDisplay(), this->visual, 24, ZPixmap, 0, reinterpret_cast<char*>(this->pixelBuffer.data()), this->win->getWidth(), this->win->getHeight(), 32, 0);

    }
    void Destroy(){
        //call before destroying the window!
        this->x_image->data = nullptr; //sets the data to nullptr becuz vector will automatically handle cleaning up the pixel buffer
        XDestroyImage(this->x_image); //destroys the image that we blits on the screen
        XFreeGC(this->win->getDisplay(), this->gc); //frees the graphics context(i fucking dont know why but x11 wants it for smreason to use the XImageFunction);
    }

    //clearing the pixel buffer
    void ClearBuffer(){
        for(int i = 0; i < this->win->getWidth() * this->win->getHeight(); ++i){
            this->pixelBuffer[i] = 0x00000000;
        }
    }

    //plotting a pixel on screen
    void PutPixel(const Xvec2* vec2, uint8_t r, uint8_t g, uint8_t b, uint8_t a){ //vector holding x and y coords
        if(vec2->x >= 0 && vec2->y >= 0 && vec2->x < this->win->getWidth() && vec2->y < this->win->getHeight()){
            this->pixelBuffer[(vec2->y * this->win->getWidth()) + vec2->x] = colorCode(r, g, b, a);
        }
    }
    void PutPixel(const Xvec2* vec2, uint32_t color){ //vector holding x and y coords
        if(vec2->x >= 0 && vec2->y >= 0 && vec2->x < this->win->getWidth() && vec2->y < this->win->getHeight()){
            this->pixelBuffer[(vec2->y * this->win->getWidth()) + vec2->x] = color;
        }
    }
    void PutPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){ //overloaded function of putpixel with raw x and y coords
        if(x >= 0 && y >= 0 && x < this->win->getWidth() && y < this->win->getHeight()){
            this->pixelBuffer[(y * this->win->getWidth()) + x] = colorCode(r, g, b, a);
        }
    }
    void PutPixel(int x, int y, uint32_t color){ //overloaded function of putpixel with raw x and y coords
        if(x >= 0 && y >= 0 && x < this->win->getWidth() && y < this->win->getHeight()){
            this->pixelBuffer[(y * this->win->getWidth()) + x] = color;
        }
    }
    void PutPixelAlpha(int x, int y, uint32_t src_color){
        if(x < 0 || y < 0 || y >= this->win->getHeight() || x >= this->win->getWidth() ) return;

        //extracting src alpha value from the src_color
        uint8_t src_a { static_cast<uint8_t>((src_color >> 24) & 0xFF) };

        //simple optimizations to skip the math
        if(src_a == 0) return;
        if(src_a == 255){
            this->PutPixel(x, y, src_color);
            return;
        }

        //converting src_a(0-255) -> alpha(0.0-1.0) and inverse alpha
        float alpha { static_cast<float>(src_a) / 255.0f };
        float inv_alpha { 1.0f - alpha };

        uint32_t dst_color { this->getScreenPixel(x, y) };

        //extracting RBG from source and destination pixels
        uint8_t src_r { static_cast<uint8_t>((src_color >> 16 ) & 0xFF) };
        uint8_t src_g { static_cast<uint8_t>((src_color >> 8 )  & 0xFF) };
        uint8_t src_b { static_cast<uint8_t>((src_color)        & 0xFF) };

        uint8_t dst_r { static_cast<uint8_t>((dst_color >> 16)  & 0xFF) };
        uint8_t dst_g { static_cast<uint8_t>((dst_color >> 8 )  & 0XFF) };
        uint8_t dst_b { static_cast<uint8_t>((dst_color)        & 0xFF) };

        //final color, blending the src RGB channel with dst RBG channel by multiplying with alpha value
        uint8_t final_r { static_cast<uint8_t>((src_r * alpha) + (dst_r * inv_alpha)) };
        uint8_t final_g { static_cast<uint8_t>((src_g * alpha) + (dst_g * inv_alpha)) };
        uint8_t final_b { static_cast<uint8_t>((src_b * alpha) + (dst_b * inv_alpha)) };

        uint32_t final_color { this->colorCode(final_r, final_g, final_b, 255) };

        this->PutPixel(x, y, final_color);
    }

    //drawing a rect
    void drawRect(const XRect* rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a){

        for(int i = rect->y; i < rect->y + rect->h; ++i){
            for(int j = rect->x; j < rect->x + rect->w; ++j){
                this->PutPixelAlpha(j, i, this->colorCode(r, g, b, a));
            }
        }
    }
    void drawRect(const XFRect* rect, uint8_t r, uint8_t g, uint8_t b, uint8_t a){

        for(int i = rect->y; i < static_cast<int>(rect->y + rect->h); ++i){
            for(int j = rect->x; j < static_cast<int>(rect->x + rect->w); ++j){
                this->PutPixelAlpha(j, i, this->colorCode(r, g, b, a));
            }
        }
    }
    void drawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
        for(int i = y; i < y + h; ++i){
            for(int j = x; j < x + w; ++j){
                this->PutPixelAlpha(j, i, this->colorCode(r, g, b, a));
            }
        }
    }
    void drawRect(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
        for(int i = y; i < static_cast<int>(y + h); ++i){
            for(int j = x; j < static_cast<int>(x + w); ++j){
                this->PutPixelAlpha(j, i, this->colorCode(r, g, b, a));
            }
        }
    }

    //drawing texture
    void drawTexture(Texture* texture, const XRect* rect){
        if(texture->getPixelData()->size() <= 0) return;

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < rect->h; ++y){
            for(int x = 0; x < rect->w; ++x){

                float u { static_cast<float>(x) / static_cast<float>(rect->w) };
                float v { static_cast<float>(y) / static_cast<float>(rect->h) };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { rect->x + x };
                int screen_y { rect->y + y };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTexture(Texture* texture, int startX, int startY, int width, int height){
        if(texture->getPixelData()->size() <= 0) return;

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){

                float u { static_cast<float>(x) / static_cast<float>(width) };
                float v { static_cast<float>(y) / static_cast<float>(height) };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { startX + x };
                int screen_y { startY + y };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTexture(Texture* texture, const XFRect* rect){
        if(texture->getPixelData()->size() <= 0) return;

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < static_cast<int>(rect->h); ++y){
            for(int x = 0; x < static_cast<int>(rect->w); ++x){

                float u { static_cast<float>(x) / rect->w };
                float v { static_cast<float>(y) / rect->h };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { static_cast<int>(std::floorf(rect->x + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(rect->y + static_cast<float>(y))) };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTexture(Texture* texture, float startX, float startY, float width, float height){
        if(texture->getPixelData()->size() <= 0) return;

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < static_cast<int>(height); ++y){
            for(int x = 0; x < static_cast<int>(width); ++x){

                float u { static_cast<float>(x) / width };
                float v { static_cast<float>(y) / height };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { static_cast<int>(std::floorf(startX + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(startY + static_cast<float>(y))) };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }

    void drawTextureScaled(Texture* texture, int startX, int startY, int width, int height, float scale_factor){
        if(texture->getPixelData()->size() <= 0) return;

        width = static_cast<int>(static_cast<float>(width) * scale_factor);
        height = static_cast<int>(static_cast<float>(height) * scale_factor);

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){

                float u { static_cast<float>(x) / static_cast<float>(width) };
                float v { static_cast<float>(y) / static_cast<float>(height) };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { startX + x };
                int screen_y { startY + y };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTextureScaled(Texture* texture, const XRect* rect, float scale_factor){ //overloaded function for drawing texture scaled with rect
        if(texture->getPixelData()->size() <= 0) return;

        int width { static_cast<int>(static_cast<float>(rect->w) * scale_factor)};
        int height { static_cast<int>(static_cast<float>(rect->h) * scale_factor)};

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < width; ++y){
            for(int x = 0; x < height; ++x){

                float u { static_cast<float>(x) / static_cast<float>(width) };
                float v { static_cast<float>(y) / static_cast<float>(height) };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { rect->x + x };
                int screen_y { rect->y + y };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTextureScaled(Texture* texture, float startX, float startY, float width, float height, float scale_factor){
        if(texture->getPixelData()->size() <= 0) return;

        width = width * scale_factor;
        height = height * scale_factor;

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < height; ++y){
            for(int x = 0; x < width; ++x){

                float u { static_cast<float>(x) / width };
                float v { static_cast<float>(y) / height };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { static_cast<int>(std::floorf(startX + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(startY + static_cast<float>(y))) };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }
    void drawTextureScaled(Texture* texture, const XFRect* rect, float scale_factor){ //overloaded function for drawing texture scaled with rect
        if(texture->getPixelData()->size() <= 0) return;

        float width { rect->w * scale_factor};
        float height { rect->h * scale_factor};

        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        for(int y = 0; y < width; ++y){
            for(int x = 0; x < height; ++x){

                float u { static_cast<float>(x) / width };
                float v { static_cast<float>(y) / height };

                int tex_x { static_cast<int>(u * static_cast<float>(tex_width)) };
                int tex_y { static_cast<int>(v * static_cast<float>(tex_height)) };

                if(tex_x >= tex_width) tex_x = tex_width - 1;
                if(tex_y >= tex_height) tex_y = tex_height - 1;

                int screen_x { static_cast<int>(std::floorf(rect->x + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(rect->y + static_cast<float>(y))) };

                uint32_t color { texture->getPixel(tex_x, tex_y) };

                this->PutPixelAlpha(screen_x, screen_y, color);
            }
        }
    }

    //drawing texture rotated 
    void drawTextureRotated(Texture* texture, int xPos, int yPos, int width, int height, float angle){
        if(texture->getPixelData()->size() <= 0) return; //checking if the texture is loaded to prevent undefined behaviour

        //converting angle in degrees to radians and gettings the cos and sin of the angle
        float angle_rad { (angle) * 3.14159f / 180.0f };
        float sin_a {std::sinf(angle_rad) };
        float cos_a { std::cosf(angle_rad) };

        //tex width and height
        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        //center position of the rect
        float centerX { static_cast<float>(xPos) + static_cast<float>(width) / 2.0f };
        float centerY { static_cast<float>(yPos) + static_cast<float>(height) / 2.0f };

        //calculating the bounding box(js a optimization)
        float diagonal { std::sqrtf(static_cast<float>(width * width + height * height)) };
        int box_size { static_cast<int>(std::ceilf(diagonal)) };

        //finding the start x and y pos in the screen for the bounding box
        int startX { static_cast<int>(centerX) - box_size / 2 };
        int startY { static_cast<int>(centerY) - box_size / 2 };

        //finding the scaling ratios
        float scale_x { static_cast<float>(width) / static_cast<float>(tex_width) };
        float scale_y { static_cast<float>(height) / static_cast<float>(tex_height) };

        //looping through the bounding box for draw the rotated texture inside the bounding box
        for(int y = 0; y < box_size; ++y){
            for(int x = 0; x < box_size; ++x){

                int screen_x { startX + x };
                int screen_y { startY + y };

                float local_x { static_cast<float>(screen_x - centerX) };
                float local_y { static_cast<float>(screen_y - centerY) };

                float rotation_x { (local_x * cos_a) + (local_y * sin_a) };
                float rotation_y { (-local_x * sin_a) + (local_y * cos_a) };

                int tex_x { static_cast<int>((rotation_x / scale_x) + static_cast<float>(tex_width) / 2.0f) };
                int tex_y { static_cast<int>((rotation_y / scale_y) + static_cast<float>(tex_height) / 2.0f) }; 

                if(tex_x >= 0 && tex_y >= 0 && tex_x < tex_width && tex_y < tex_height){

                    uint32_t color { texture->getPixel(tex_x, tex_y) };

                    this->PutPixelAlpha(screen_x, screen_y, color);
                }
            }
        }
    }
    void drawTextureRotated(Texture* texture, const XRect* rect, float angle){
        if(texture->getPixelData()->size() <= 0) return; //checking if the texture is loaded to prevent undefined behaviour

        //converting angle in degrees to radians and gettings the cos and sin of the angle
        float angle_rad { (angle) * 3.14159f / 180.0f };
        float sin_a {std::sinf(angle_rad) };
        float cos_a { std::cosf(angle_rad) };

        //tex width and height
        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        //center position of the rect
        float centerX { static_cast<float>(rect->x) + static_cast<float>(rect->w) / 2.0f };
        float centerY { static_cast<float>(rect->y) + static_cast<float>(rect->h) / 2.0f };

        //calculating the bounding box(js a optimization)
        float diagonal { std::sqrtf(static_cast<float>(rect->w * rect->w + rect->h * rect->h)) };
        int box_size { static_cast<int>(std::ceilf(diagonal)) };

        //finding the start x and y pos in the screen for the bounding box
        int startX { static_cast<int>(centerX) - box_size / 2 };
        int startY { static_cast<int>(centerY) - box_size / 2 };

        //finding the scaling ratios
        float scale_x { static_cast<float>(rect->w) / static_cast<float>(tex_width) };
        float scale_y { static_cast<float>(rect->h) / static_cast<float>(tex_height) };

        //looping through the bounding box for draw the rotated texture inside the bounding box
        for(int y = 0; y < box_size; ++y){
            for(int x = 0; x < box_size; ++x){

                int screen_x { startX + x };
                int screen_y { startY + y };

                float local_x { static_cast<float>(screen_x - centerX) };
                float local_y { static_cast<float>(screen_y - centerY) };

                float rotation_x { (local_x * cos_a) + (local_y * sin_a) };
                float rotation_y { (-local_x * sin_a) + (local_y * cos_a) };

                int tex_x { static_cast<int>((rotation_x / scale_x) + static_cast<float>(tex_width) / 2.0f) };
                int tex_y { static_cast<int>((rotation_y / scale_y) + static_cast<float>(tex_height) / 2.0f) }; 

                if(tex_x >= 0 && tex_y >= 0 && tex_x < tex_width && tex_y < tex_height){

                    uint32_t color { texture->getPixel(tex_x, tex_y) };

                    this->PutPixelAlpha(screen_x, screen_y, color);
                }
            }
        }
    }
    void drawTextureRotated(Texture* texture, float xPos, float yPos, float width, float height, float angle){
        if(texture->getPixelData()->size() <= 0) return; //checking if the texture is loaded to prevent undefined behaviour

        //converting angle in degrees to radians and gettings the cos and sin of the angle
        float angle_rad { (angle) * 3.14159f / 180.0f };
        float sin_a {std::sinf(angle_rad) };
        float cos_a { std::cosf(angle_rad) };

        //tex width and height
        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        //center position of the rect
        float centerX { xPos + width / 2.0f };
        float centerY { yPos + height / 2.0f };

        //calculating the bounding box(js a optimization)
        float diagonal { width * width + height * height };
        int box_size { static_cast<int>(std::ceilf(diagonal)) };

        //finding the start x and y pos in the screen for the bounding box
        float startX { centerX - box_size / 2.0f };
        float startY { centerY - box_size / 2.0f };

        //finding the scaling ratios
        float scale_x { width / static_cast<float>(tex_width) };
        float scale_y { height / static_cast<float>(tex_height) };

        //looping through the bounding box for draw the rotated texture inside the bounding box
        for(int y = 0; y < box_size; ++y){
            for(int x = 0; x < box_size; ++x){

                int screen_x { static_cast<int>(std::floorf(startX + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(startY + static_cast<float>(y))) };

                float local_x { static_cast<float>(screen_x - centerX) };
                float local_y { static_cast<float>(screen_y - centerY) };

                float rotation_x { (local_x * cos_a) + (local_y * sin_a) };
                float rotation_y { (-local_x * sin_a) + (local_y * cos_a) };

                int tex_x { static_cast<int>((rotation_x / scale_x) + static_cast<float>(tex_width) / 2.0f) };
                int tex_y { static_cast<int>((rotation_y / scale_y) + static_cast<float>(tex_height) / 2.0f) }; 

                if(tex_x >= 0 && tex_y >= 0 && tex_x < tex_width && tex_y < tex_height){

                    uint32_t color { texture->getPixel(tex_x, tex_y) };

                    this->PutPixelAlpha(screen_x, screen_y, color);
                    
                }
            }
        }
    }   
    void drawTextureRotated(Texture* texture, const XFRect* rect, float angle){
        if(texture->getPixelData()->size() <= 0) return; //checking if the texture is loaded to prevent undefined behaviour

        //converting angle in degrees to radians and gettings the cos and sin of the angle
        float angle_rad { (angle) * 3.14159f / 180.0f };
        float sin_a {std::sinf(angle_rad) };
        float cos_a { std::cosf(angle_rad) };

        //tex width and height
        int tex_width { texture->getW() };
        int tex_height { texture->getH() };

        //center position of the rect
        float centerX { rect->x + rect->w / 2.0f };
        float centerY { rect->y + rect->h / 2.0f };

        //calculating the bounding box(js a optimization)
        float diagonal { std::sqrtf(rect->w * rect->w + rect->h * rect->h) };
        int box_size { static_cast<int>(std::ceilf(diagonal)) };

        //finding the start x and y pos in the screen for the bounding box
        float startX { centerX - box_size / 2 };
        float startY { centerY - box_size / 2 };

        //finding the scaling ratios
        float scale_x { rect->w / static_cast<float>(tex_width) };
        float scale_y { rect->h / static_cast<float>(tex_height) };

        //looping through the bounding box for draw the rotated texture inside the bounding box
        for(int y = 0; y < box_size; ++y){
            for(int x = 0; x < box_size; ++x){

                int screen_x { static_cast<int>(std::floorf(startX + static_cast<float>(x))) };
                int screen_y { static_cast<int>(std::floorf(startY + static_cast<float>(y))) };

                float local_x { static_cast<float>(screen_x - centerX) };
                float local_y { static_cast<float>(screen_y - centerY) };

                float rotation_x { (local_x * cos_a) + (local_y * sin_a) };
                float rotation_y { (-local_x * sin_a) + (local_y * cos_a) };

                int tex_x { static_cast<int>((rotation_x / scale_x) + static_cast<float>(tex_width) / 2.0f) };
                int tex_y { static_cast<int>((rotation_y / scale_y) + static_cast<float>(tex_height) / 2.0f) }; 

                if(tex_x >= 0 && tex_y >= 0 && tex_x < tex_width && tex_y < tex_height){

                    uint32_t color { texture->getPixel(tex_x, tex_y) };

                    this->PutPixelAlpha(screen_x, screen_y, color);
                }
            }
        }
    }

    //swaping buffers
    void SwapRenderBuffers(){
        //draws the pixel buffer to the x_image struct
        XPutImage(this->win->getDisplay(), *this->win->getRawWindow(), this->gc, this->x_image, 0, 0, 0, 0, this->win->getWidth(), this->win->getHeight());
        XFlush(this->win->getDisplay()); //flushes the pending commands in the window so the program runs smoothly!
    }

    private:
    uint32_t colorCode(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
        return (static_cast<uint32_t>(a) << 24 | static_cast<uint32_t>(r) << 16 | static_cast<uint32_t>(g) << 8 | static_cast<uint32_t>(g));
    }

    uint32_t getScreenPixel(int x, int y){
        //returning the current pixel on the sreen at x, y coordinates
        if(x >= 0 && y >= 0 && x < this->win->getWidth() && y < this->win->getHeight()){
            return this->pixelBuffer[(y * this->win->getWidth()) + x];
        }

        return 0x00000000;
    }
};