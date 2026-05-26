#pragma once

#include <cstdint>

//vectors
typedef struct{
    int x, y;
}  Xvec2;

typedef struct{
    float x, y;
} XFvec2;


//rects
typedef struct {
    int x, y, w, h;
} XRect; 

typedef struct{
    float x, y, w, h;
} XFRect;