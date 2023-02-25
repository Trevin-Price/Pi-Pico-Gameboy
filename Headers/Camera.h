#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "math.h"

#define deg2rad M_PI/180.0

class Camera {
public:
    void CalculateMatrices();
    Camera();

    Vector3 position = Vector3(0, 0, 0);
    Vector3 rotation = Vector3(0, 0, 0); // degrees

    Matrix overallMatrix = Matrix(4, 4);
private:
    Matrix positionMatrix = Matrix(4, 4);
    Matrix rotationMatrix1 = Matrix(4, 4);
    Matrix rotationMatrix2 = Matrix(4, 4);
    Matrix rotationMatrix3 = Matrix(4, 4);
};