#pragma once

#include "pico/stdlib.h"

class Matrix {
    public:
        Matrix(uint8_t width, uint8_t height);

        double* operator[](uint8_t index);
        Matrix operator*(Matrix& m1);

        uint8_t width, height;
    private:
        double **matrixValues;
};