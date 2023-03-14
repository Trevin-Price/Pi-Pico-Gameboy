#include "Camera.h"

Camera::Camera() {
    Camera::positionMatrix[0][0] = 1;
    Camera::positionMatrix[1][1] = 1;
    Camera::positionMatrix[2][2] = 1;
    Camera::positionMatrix[3][3] = 1;

    Camera::rotationMatrix1[0][0] = 1;
    Camera::rotationMatrix1[3][3] = 1;

    Camera::rotationMatrix2[1][1] = 1;
    Camera::rotationMatrix2[2][2] = 1;
    Camera::rotationMatrix2[3][3] = 1;

    Camera::rotationMatrix3[2][2] = 1;
    Camera::rotationMatrix3[3][3] = 1;
}

void Camera::CalculateMatrices() {
    positionMatrix[3][0] = -Position.x;
    positionMatrix[3][1] = -Position.y;
    positionMatrix[3][2] = -Position.z;

    Vector3 cameraRotationRadians = Rotation * deg2rad;

    rotationMatrix1[1][1] = cos(cameraRotationRadians.x);
    rotationMatrix1[1][2] = sin(cameraRotationRadians.x);
    rotationMatrix1[2][1] = -sin(cameraRotationRadians.x);
    rotationMatrix1[2][2] = cos(cameraRotationRadians.x);

    rotationMatrix2[0][0] = cos(cameraRotationRadians.y);
    rotationMatrix2[1][0] = sin(cameraRotationRadians.y);
    rotationMatrix2[0][2] = -sin(cameraRotationRadians.y);
    rotationMatrix2[1][2] = cos(cameraRotationRadians.y);

    rotationMatrix3[0][0] = cos(cameraRotationRadians.z);
    rotationMatrix3[1][0] = -sin(cameraRotationRadians.z);
    rotationMatrix3[0][1] = sin(cameraRotationRadians.z);
    rotationMatrix3[1][1] = cos(cameraRotationRadians.z);

    overallMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3 * positionMatrix;
}