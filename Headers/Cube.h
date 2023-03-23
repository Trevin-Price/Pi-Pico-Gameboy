#pragma once

#include "Vector.h"
#include "Colors.h"
#include "pico/stdlib.h"

class Cube {
public:
    Cube(Vector3 position, Vector3 rotation, Vector3 size) {
        Position = position;
        Rotation = rotation;
        Size = size;
    }

    Vector3 Position = Vector3(0, 0, 0); // center of cube
    Vector3 Rotation = Vector3(0, 0, 0); // pitch, yaw, roll
    Vector3 Size = Vector3(1, 1, 1); // expanded by size/2 in each axis, in both forward and reverse
    // ex: cube at 0,0,0, with size 1,1,1 will be from -0.5, -0.5, -0.5 to 0.5, 0.5, 0.5
    uint16_t FaceColor = Black;
    uint16_t EdgeColor = White;
};
