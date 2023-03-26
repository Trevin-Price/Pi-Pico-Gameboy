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
Matrix::Matrix(const Vector3 v1) {
    width = 1;
    height = 3;
    matrixValues = new double*[1];
    matrixValues[0] = new double[3];
    
    matrixValues[0][0] = v1.X;
    matrixValues[0][1] = v1.Y;
    matrixValues[0][2] = v1.Z;
}
Matrix::Matrix(const Matrix& m1) {
    width = m1.width;
    height = m1.height;

    matrixValues = new double*[width];
    for (uint8_t x = 0; x < width; x++) {
        matrixValues[x] = new double[height];
        for (uint8_t y = 0; y < height; y++)
            matrixValues[x][y] = m1[x][y];
    }
}

Matrix::~Matrix() { // previously forgot to add Matrix deconstructor, which was not ideal for memory usage
    for (uint8_t x = 0; x < width; x++)
        delete[] matrixValues[x];
    delete[] matrixValues;
}

double* Matrix::operator [](uint8_t index) const {
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

void Matrix::operator =(const Vector3& v1) {
    if (this->width == 1 &&this->height == 3) {
        this->matrixValues[0][0] = v1.X;
        this->matrixValues[0][1] = v1.Y;
        this->matrixValues[0][2] = v1.Z;
    }
}

Matrix Matrix::operator *(const Matrix& m1) const {
    Matrix productMatrix = Matrix(m1.width, height);
    
    if (m1.height != width) {
        productMatrix.width = 0;
        productMatrix.height = 0;
    }

    for (uint8_t y = 0; y < productMatrix.height; y++) {
        for (uint8_t x = 0; x < productMatrix.width; x++) {
            double newVal = 0;
            for (uint8_t x2 = 0; x2 < width; x2++) {
                if (x2 >= m1.height) 
                    break;
                newVal += (matrixValues[x2][y] * m1[x][x2]);
            }
            productMatrix[x][y] = newVal;
        }
    }

    return productMatrix;
}

std::string Matrix::toString() {

    if (width == 0 || height == 0)
        return "UNDEFINED MATRIX";

    std::string myStr = "";

    for (uint8_t x = 0; x < width; x++) {
        for (uint8_t y = 0; y < height; y++)
            myStr = myStr.append(std::to_string(x).append("x").append(std::to_string(y)).append(" = ").append(std::to_string(matrixValues[x][y])).append("\n"));
    }

    return myStr;
}