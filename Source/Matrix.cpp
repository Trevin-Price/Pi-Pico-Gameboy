#include "Matrix.h"

Matrix::Matrix(uint8_t width, uint8_t height) {
    Matrix::width = width;
    Matrix::height = height;
    matrixValues = new double*[width];
    for (int x = 0; x < width; x++) {
        matrixValues[x] = new double[height];
        for (int y = 0; y < height; y++)
            matrixValues[x][y] = 0;
    }
}

double* Matrix::operator [](uint8_t index) {
    if (index > Matrix::width) {
        // add error here
    }

    return Matrix::matrixValues[index];
}

Matrix Matrix::operator *(Matrix& m1) {
    Matrix productMatrix = Matrix(this->width < m1.width ? this->width : m1.width, this->height < m1.height ? this->height : m1.height);
    
    for (uint8_t y = 0; y < productMatrix.height; y++) {
        for (uint8_t x = 0; x < productMatrix.width; x++) {
            double newVal = 0;
            for (uint8_t m1x = 0; m1x < this->width; m1x++) {
                if (m1x >= m1.height) 
                    break;
                newVal += (this->matrixValues[m1x][y] * m1.matrixValues[x][m1x]);
            }
            productMatrix.matrixValues[x][y] = newVal;
        }
    }

    return productMatrix;
}