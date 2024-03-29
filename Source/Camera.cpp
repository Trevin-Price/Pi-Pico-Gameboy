#include "Camera.h"

Camera::Camera() {
    positionMatrix[0][0] = 1;
    positionMatrix[1][1] = 1;
    positionMatrix[2][2] = 1;
    positionMatrix[3][3] = 1;

    subRotationMatrix1[0][0] = 1;
    subRotationMatrix1[3][3] = 1;

    subRotationMatrix2[1][1] = 1;
    //subRotationMatrix2[2][2] = 1;
    subRotationMatrix2[3][3] = 1;

    subRotationMatrix3[2][2] = 1;
    subRotationMatrix3[3][3] = 1;

    //displayMatrix[1][2] = -1;
    displayMatrix[2][2] = -1; // 1
    displayMatrix[3][3] = 1;

    objectPositionMatrix[0][3] = 1;
    depthMatrix[2][2] = 1;
    depthMatrix[3][3] = 1;
    
    updateSensorSize();
    updatePositionAndRotation(); // init display matrix
}

void Camera::updateSensorSize() {
    sensorSize = Vector2(tan(horizontalFOV/2*deg2rad)*focalLength, tan((double)DISPLAY_HEIGHT/(double)DISPLAY_WIDTH*horizontalFOV/2*deg2rad)*focalLength);

    displayMatrix[0][0] = focalLength * DISPLAY_WIDTH / (2 * sensorSize.X);
    displayMatrix[1][1] = focalLength * DISPLAY_HEIGHT / (2 * sensorSize.Y);
    displayMatrix[1][0] = skew;

    overallMatrix = displayMatrix * rotationMatrix * positionMatrix;
}

void Camera::updatePosition() {
    positionMatrix[3][0] = -Position.X;
    positionMatrix[3][1] = -Position.Y;
    positionMatrix[3][2] = -Position.Z;

    overallMatrix = rotationWithDisplayMatrix * positionMatrix;
}

void Camera::updateRotation() {
    Vector3 cameraRotationRadians = Rotation * deg2rad;

    subRotationMatrix1[1][1] = cos(cameraRotationRadians.X);
    subRotationMatrix1[1][2] = sin(cameraRotationRadians.X);
    subRotationMatrix1[2][1] = -sin(cameraRotationRadians.X);
    subRotationMatrix1[2][2] = cos(cameraRotationRadians.X);

    subRotationMatrix2[0][0] = cos(cameraRotationRadians.Y);
    subRotationMatrix2[2][0] = sin(cameraRotationRadians.Y);
    subRotationMatrix2[0][2] = -sin(cameraRotationRadians.Y);
    subRotationMatrix2[2][2] = cos(cameraRotationRadians.Y);

    subRotationMatrix3[0][0] = cos(cameraRotationRadians.Z);
    subRotationMatrix3[1][0] = -sin(cameraRotationRadians.Z);
    subRotationMatrix3[0][1] = sin(cameraRotationRadians.Z);
    subRotationMatrix3[1][1] = cos(cameraRotationRadians.Z);

    rotationMatrix = subRotationMatrix3 * subRotationMatrix2 * subRotationMatrix1;
    rotationWithDisplayMatrix = displayMatrix * rotationMatrix;

    overallMatrix = rotationWithDisplayMatrix * positionMatrix;
}

void Camera::updatePositionAndRotation() {
    positionMatrix[3][0] = -Position.X;
    positionMatrix[3][1] = -Position.Y;
    positionMatrix[3][2] = -Position.Z;

    Vector3 cameraRotationRadians = Rotation * deg2rad;

    subRotationMatrix1[1][1] = cos(cameraRotationRadians.X);
    subRotationMatrix1[1][2] = sin(cameraRotationRadians.X);
    subRotationMatrix1[2][1] = -sin(cameraRotationRadians.X);
    subRotationMatrix1[2][2] = cos(cameraRotationRadians.X);

    subRotationMatrix2[0][0] = cos(cameraRotationRadians.Y);
    subRotationMatrix2[2][0] = sin(cameraRotationRadians.Y);
    subRotationMatrix2[0][2] = -sin(cameraRotationRadians.Y);
    subRotationMatrix2[2][2] = cos(cameraRotationRadians.Y);

    subRotationMatrix3[0][0] = cos(cameraRotationRadians.Z);
    subRotationMatrix3[1][0] = -sin(cameraRotationRadians.Z);
    subRotationMatrix3[0][1] = sin(cameraRotationRadians.Z);
    subRotationMatrix3[1][1] = cos(cameraRotationRadians.Z);

    rotationMatrix = subRotationMatrix3 * subRotationMatrix2 * subRotationMatrix1;
    rotationWithDisplayMatrix = displayMatrix * rotationMatrix;

    overallMatrix = rotationWithDisplayMatrix * positionMatrix;
}

bool Camera::project3DTo2D(Vector3 point, Vector2 *pos) {
    objectPositionMatrix[0][0] = point.X;
    objectPositionMatrix[0][1] = point.Y;
    objectPositionMatrix[0][2] = point.Z;
    Matrix screenPosMatrix = overallMatrix * objectPositionMatrix;

    double depthMultiplier = 1/screenPosMatrix[0][2];
    depthMatrix[0][0] = depthMultiplier;
    depthMatrix[1][1] = depthMultiplier;
    screenPosMatrix = depthMatrix * screenPosMatrix; // account for depth (add perspective)
    
    pos->X = (screenPosMatrix[0][0] + DISPLAY_WIDTH/2);
    pos->Y = (-screenPosMatrix[0][1] + DISPLAY_HEIGHT/2);

    return (screenPosMatrix[0][2] >= 0 && pos->X < DISPLAY_WIDTH && pos->X >= 0 && pos->Y < DISPLAY_HEIGHT && pos->Y >= 0) ? true : false;

    // the comments below are old, but I'm keeping them as a record of some of the issues I ran into while working on this project

    // DISPLAY_ - X is a stupid fix
    // this is an issue to do with the order I multiply matrices, or something
    // basically, prior to accounting for depth, matrix[0][2] is the negative version of what it should be
    // because I couldn't figure out why after 4 hours of trying, I slapped on a bandaid fix and called it a day
    // I could optimize this math, but to show that it's a bandaid fix, I'm leaving it as is

    // there is also another issue related to y coordinate not working correctly, and this is likely related
    // after fixing the y coordinate issue, I realized the problem was that I was incorrectly rounding the double coordinate values into integers, causing it to give incorrect screen coordinates
    
    // after further investigation, I disovered that the problem was that the video I was watching had modified the math they were citing, despite not mentioning this (or me failing to notice their mention of it)
    // modifying my code to match theirs fixed the issues listed above, as well as others I hadn't documented
    // I'd like to go back and rewrite my code to match the cited math while achieving the desired result, so that I can fully understand why their code differed from their cited math
}