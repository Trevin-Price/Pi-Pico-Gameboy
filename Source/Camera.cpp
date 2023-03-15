#include "Camera.h"

Camera::Camera() {
    positionMatrix[0][0] = 1;
    positionMatrix[1][1] = 1;
    positionMatrix[2][2] = 1;
    positionMatrix[3][3] = 1;

    rotationMatrix1[0][0] = 1;
    rotationMatrix1[3][3] = 1;

    rotationMatrix2[1][1] = 1;
    rotationMatrix2[2][2] = 1;
    rotationMatrix2[3][3] = 1;

    rotationMatrix3[2][2] = 1;
    rotationMatrix3[3][3] = 1;

    displayMatrix[1][2] = -1;
    displayMatrix[2][2] = 1;
    displayMatrix[3][3] = 1;

    objectPositionMatrix[0][3] = 1;
    depthMatrix[2][2] = 1;
    depthMatrix[3][3] = 1;
    
    calculateSensorSize();
    calculatePositionAndRotationMatrices(); // init display matrix
}

void Camera::calculateSensorSize() {
    sensorSize = Vector2(tan(horizontalFOV/2*deg2rad)*focalLength, tan((double)DISPLAY_HEIGHT/(double)DISPLAY_WIDTH*horizontalFOV/2*deg2rad)*focalLength);

    displayMatrix[0][0] = focalLength * DISPLAY_WIDTH / (2 * sensorSize.X);
    displayMatrix[1][1] = focalLength * DISPLAY_HEIGHT / (2 * sensorSize.Y);
    displayMatrix[1][0] = skew;
}

void Camera::calculatePositionAndRotationMatrices() {
    positionMatrix[3][0] = -Position.X;
    positionMatrix[3][1] = -Position.Y;
    positionMatrix[3][2] = -Position.Z;

    Vector3 cameraRotationRadians = Rotation * deg2rad;

    rotationMatrix1[1][1] = cos(cameraRotationRadians.X);
    rotationMatrix1[1][2] = sin(cameraRotationRadians.X);
    rotationMatrix1[2][1] = -sin(cameraRotationRadians.X);
    rotationMatrix1[2][2] = cos(cameraRotationRadians.X);

    rotationMatrix2[0][0] = cos(cameraRotationRadians.Y);
    rotationMatrix2[1][0] = sin(cameraRotationRadians.Y);
    rotationMatrix2[0][2] = -sin(cameraRotationRadians.Y);
    rotationMatrix2[1][2] = cos(cameraRotationRadians.Y);

    rotationMatrix3[0][0] = cos(cameraRotationRadians.Z);
    rotationMatrix3[1][0] = -sin(cameraRotationRadians.Z);
    rotationMatrix3[0][1] = sin(cameraRotationRadians.Z);
    rotationMatrix3[1][1] = cos(cameraRotationRadians.Z);

    overallMatrix = rotationMatrix1 * rotationMatrix2 * rotationMatrix3 * positionMatrix;
}

bool Camera::project3DTo2D(Vector3 point, Vector2 *pos) {
    objectPositionMatrix[0][0] = point.X;
    objectPositionMatrix[0][1] = point.Y;
    objectPositionMatrix[0][2] = point.Z;

    Matrix screenPosMatrix = displayMatrix * overallMatrix * objectPositionMatrix;

    double depthMultiplier = 1/screenPosMatrix[0][2];
    depthMatrix[0][0] = depthMultiplier;
    depthMatrix[1][1] = depthMultiplier;
    screenPosMatrix = screenPosMatrix * depthMatrix; // account for depth (add perspective)
    
    if (screenPosMatrix[0][2] > 0) { // make sure it's in front of the camera
        pos->X = DISPLAY_WIDTH - (screenPosMatrix[0][0] + DISPLAY_WIDTH/2);
        pos->Y = DISPLAY_HEIGHT - (-screenPosMatrix[0][1] + DISPLAY_HEIGHT/2);
        // DISPLAY_ - X is a stupid fix
        // this is an issue to do with the order I multiply matrices, or something
        // basically, prior to accounting for depth, matrix[0][2] is the negative version of what it should be
        // because I couldn't figure out why after 4 hours of trying, I slapped on a bandaid fix and called it a day
        // I could optimize this math, but to show that it's a bandaid fix, I'm leaving it as is

        // there is also another issue related to y coordinate not working correctly, and this is likely related
        // after fixing the y coordinate issue, I realized the problem was that I was incorrectly rounding the double coordinate values into integers, causing it to give incorrect screen coordinates
        
        return true;
    }
    
    return false;
}