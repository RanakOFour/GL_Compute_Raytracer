#ifndef INPUT_H
#define INPUT_H

struct Input
{
    int forward;
    int right;
    int up;
    float deltaMouseX;
    float deltaMouseY;

    Input()
    : forward(0)
    , right(0)
    , up(0)
    , deltaMouseX(0.0f)
    , deltaMouseY(0.0f)
    {};
};

#endif