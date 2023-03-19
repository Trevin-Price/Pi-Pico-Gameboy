#include "Matrix.h"

Matrix::Matrix(uint8_t width, uint8_t height) {
    Matrix::width = width;
    Matrix::height = height;
    matrixValues = new double*[width];
    for (uint8_t x = 0; x < width; x++) {
        matrixValues[x] = new double[height];
        for (uint8_t y = 0; y < height; y++)
            matrixValues[x][y] = 0;
    }
}

double Matrix::destructCount = 0;

Matrix::~Matrix() { // previously forgot to add Matrix deconstructor, which was not ideal for memory usage
    for (uint8_t x = 0; x < width; x++)
        delete[] matrixValues[x];
    delete[] matrixValues;
}

double* Matrix::operator [](uint8_t index) {
    if (index > width) {
        // add error here
    }

    return matrixValues[index];
}

void Matrix::operator =(const Matrix& m1) {
    if (this != &m1) {
        for (uint8_t x = 0; x < width; x++)
            delete[] matrixValues[x];
        delete[] matrixValues;

        matrixValues = new double*[m1.width];
        for (uint8_t x = 0; x < m1.width; x++) {
            matrixValues[x] = new double[m1.height];
            for (uint8_t y = 0; y < m1.height; y++)
                matrixValues[x][y] = m1.matrixValues[x][y];
        }
    }
}

Matrix Matrix::operator *(const Matrix& m1) {
    Matrix productMatrix = Matrix(width < m1.width ? width : m1.width, height < m1.height ? height : m1.height);
    
    for (uint8_t y = 0; y < productMatrix.height; y++) {
        for (uint8_t x = 0; x < productMatrix.width; x++) {
            double newVal = 0;
            for (uint8_t m2x = 0; m2x < width; m2x++) {
                if (m2x >= m1.height) 
                    break;
                newVal += (matrixValues[m2x][y] * m1.matrixValues[x][m2x]);
            }
            productMatrix.matrixValues[x][y] = newVal;
        }
    }

    return productMatrix;
}

std::string Matrix::toString() {
    std::string myStr = "";

    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++)
            myStr = myStr.append(std::to_string(x).append("x").append(std::to_string(y)).append(" = ").append(std::to_string(matrixValues[x][y])).append("\n"));
    }

    return myStr;
}