#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "math.h"
#include "DisplayDriver.h"

#define deg2rad M_PI/180.0

class Camera {
public:
    Camera();

    Vector3 Position = Vector3(0, 0, 0);
    Vector3 Rotation = Vector3(0, 0, 0); // degrees, (Pitch, Yaw, Roll)
    double focalLength = 10;
    double horizontalFOV = 90; // degrees
    double skew = 0;
    Vector2 sensorSize; // calculated at construction

    Matrix overallMatrix = Matrix(4, 4);

    void calculateSensorSize();
    void calculatePositionAndRotationMatrices();
    bool project3DTo2D(Vector3 worldPoint, Vector2 *screenPointPtr);
private:
    Matrix objectPositionMatrix = Matrix(1, 4);
    
    Matrix displayMatrix = Matrix(4, 4);
    Matrix depthMatrix = Matrix(4, 4);

    Matrix positionMatrix = Matrix(4, 4);
    Matrix rotationMatrix1 = Matrix(4, 4);
    Matrix rotationMatrix2 = Matrix(4, 4);
    Matrix rotationMatrix3 = Matrix(4, 4);
};