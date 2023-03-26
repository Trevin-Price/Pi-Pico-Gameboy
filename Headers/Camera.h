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

    void updateSensorSize();
    void updatePositionAndRotation();
    void updatePosition();
    void updateRotation();
    void project3DTo2D(Vector3 worldPoint, Vector2 *screenPointPtr);
private:
    Matrix objectPositionMatrix = Matrix(1, 4);
    
    Matrix displayMatrix = Matrix(4, 4);
    Matrix depthMatrix = Matrix(4, 4);

    Matrix rotationWithDisplayMatrix = Matrix(4, 4);

    Matrix positionMatrix = Matrix(4, 4);
    Matrix rotationMatrix = Matrix(4, 4);

    Matrix subRotationMatrix1 = Matrix(4, 4);
    Matrix subRotationMatrix2 = Matrix(4, 4);
    Matrix subRotationMatrix3 = Matrix(4, 4);
};