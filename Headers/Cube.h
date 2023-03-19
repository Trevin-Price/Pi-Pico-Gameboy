#pragma once

#include "Vector.h"
#include "Colors.h"
#include "pico/stdlib.h"

class Cube {
public:
    Cube(Vector3 position, Vector3 rotation, Vector3 size, uint16_t color) {
        Position = position;
        Rotation = rotation;
        Size = size;
        Color = color;
    }

    Vector3 Position = Vector3(0, 0, 0);
    Vector3 Rotation = Vector3(0, 0, 0);
    Vector3 Size = Vector3(1, 1, 1);
    uint16_t Color = Black;
};
